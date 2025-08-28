// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilityTask_PlayMontageWithRootMotionSource.h"

#include "AbilitySystemGlobals.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/Targeting/WKTargetData.h"
#include "BlackWK/Character/WKAICharacterBase.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/Player/WKPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UWKAbilityTask_PlayMontageWithRootMotionSource::UWKAbilityTask_PlayMontageWithRootMotionSource(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MovementComponent(nullptr)
	, SectionIndex(0)
	, PlayerTarget(nullptr)
	, bContinueRootMotionWhenMontageStop(false)
	, RootMotionSourceID(0)
{
}

void UWKAbilityTask_PlayMontageWithRootMotionSource::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UWKAbilityTask_PlayMontageWithRootMotionSource, SectionIndex, COND_InitialOnly);

	constexpr FDoRepLifetimeParams RepParamsGeneralPushModel{ COND_None, REPNOTIFY_Always, true };

	DOREPLIFETIME_WITH_PARAMS_FAST(UWKAbilityTask_PlayMontageWithRootMotionSource, ActorRotator, RepParamsGeneralPushModel);
	DOREPLIFETIME_WITH_PARAMS_FAST(UWKAbilityTask_PlayMontageWithRootMotionSource, ControlRotator, RepParamsGeneralPushModel);
	DOREPLIFETIME_WITH_PARAMS_FAST(UWKAbilityTask_PlayMontageWithRootMotionSource, CharacterMoveInput, RepParamsGeneralPushModel);
	DOREPLIFETIME_WITH_PARAMS_FAST(UWKAbilityTask_PlayMontageWithRootMotionSource, PlayerTarget, RepParamsGeneralPushModel);
	DOREPLIFETIME_WITH_PARAMS_FAST(UWKAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingDistance, RepParamsGeneralPushModel);
	DOREPLIFETIME_WITH_PARAMS_FAST(UWKAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingDirection, RepParamsGeneralPushModel);
	DOREPLIFETIME_WITH_PARAMS_FAST(UWKAbilityTask_PlayMontageWithRootMotionSource, CustomTargetLocation,RepParamsGeneralPushModel);
	DOREPLIFETIME_WITH_PARAMS_FAST(UWKAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingRotation,RepParamsGeneralPushModel);
	DOREPLIFETIME_WITH_PARAMS_FAST(UWKAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingTargetActorParam,RepParamsGeneralPushModel);
	DOREPLIFETIME_WITH_PARAMS_FAST(UWKAbilityTask_PlayMontageWithRootMotionSource, bContinueRootMotionWhenMontageStop, RepParamsGeneralPushModel);
}

UWKAbilityTask_PlayMontageWithRootMotionSource* UWKAbilityTask_PlayMontageWithRootMotionSource::PlayMontageWithRootMotionSource(
	UGameplayAbility* OwningAbility,
	const FGuid AutoGuid,
	FName TaskInstanceName,
	UAnimMontage* Montage,
	int32 SectionIndex,
	float SectionStartTime,
	FGameplayTagContainer WaitEventTags,
	float PlayRate,
	float RootMotionTranslationScale,
	bool bStopWhenAbilityEnd)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(PlayRate);

	UWKAbilityTask_PlayMontageWithRootMotionSource* MyObj = NewAbilityTask<UWKAbilityTask_PlayMontageWithRootMotionSource>(OwningAbility, TaskInstanceName);
	MyObj->NetSyncGuid = AutoGuid;
	MyObj->MontageToPlay = Montage;
	MyObj->EventTags = WaitEventTags;
	MyObj->Rate = PlayRate;
	if (IsValid(Montage))
	{
		MyObj->StartSection = Montage->GetSectionName(SectionIndex);
	}
	MyObj->StartTime = SectionStartTime;
	MyObj->AnimRootMotionTranslationScale = RootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnd;
	
	MyObj->SectionIndex = SectionIndex;

	//DSLog(LogTemp,Warning, TEXT("PlayMontageWithRootMotionSource %s |%d |%.2f"),*Montage->GetName(),SectionIndex,OwningAbility->GetWorld()->GetTimeSeconds());
	
	return MyObj;
}

void UWKAbilityTask_PlayMontageWithRootMotionSource::Activate()
{
	if (!IsValid(this) || (!IsValid(MontageToPlay)))
	{
		// DSLog(LogAbilitySystem,Error, TEXT("PlayMontageWithRootMotionSource 蒙太奇不能为空 ，GA:%s"),*Ability.Get()->GetName());
		EndTask();
		return;
	}

	if (!MontageToPlay->IsValidSectionIndex(SectionIndex))
	{
		// DSLog(LogAbilitySystem,Error, TEXT("PlayMontageWithRootMotionSource 传入的SectionIndex无效 GA:%s Montage:%s SectionIndex:%d"),*Ability.Get()->GetName(),*MontageToPlay->GetName(),SectionIndex);
		EndTask();
		return;
	}

	if (MontageToPlay->RateScale!= 1.f)
	{
		// DSLog(LogAbilitySystem,Error, TEXT("PlayMontageWithRootMotionSource 蒙太奇资产上的RateScale只能为1 GA:%s Montage:%s "),*Ability.Get()->GetName(),*MontageToPlay->GetName());
		//EndTask();
		//return;
	}
	
	UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(AbilitySystemComponent.Get());
	if (!ASC)
	{
		EndTask();
		return;
	}

	if (AWKCharacterBase* OwnerAvatar = Cast<AWKCharacterBase>(GetAvatarActor()))
	{
		if (!NeedReplicateAutonomousClientParamToServer())
		{
			PrepareReplicateParams();

			if (GetAvatarActor())
			{
				GetAvatarActor()->ForceNetUpdate();
			}
			
			SharedInitAndApply();
			return Super::Activate();
		}

		//需要同步主控端的朝向等数据到服务器
		FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());
		if (IsPredictingClient())
		{
			FGameplayAbilityTargetData_PlayMontage* TargetData = new FGameplayAbilityTargetData_PlayMontage();

			PrepareReplicateParams();
			
			TargetData->ActorRotator = ActorRotator;
			TargetData->ControlRotator = ControlRotator;
			TargetData->MoveInPut = CharacterMoveInput;
			TargetData->TargetActor = PlayerTarget;

			TargetData->OverrideWarpingDistance = OverrideWarpingDistance;
			TargetData->OverrideWarpingDirection = OverrideWarpingDirection;
			TargetData->CustomTargetLocation = CustomTargetLocation;
			TargetData->OverrideWarpingRotation = OverrideWarpingRotation;
			TargetData->OverrideWarpingTargetActorParam = OverrideWarpingTargetActorParam;

			ASC->ServerSetReplicatedTargetDataEx(GetAbilitySpecHandle(), NetSyncGuid, FGameplayAbilityTargetDataHandle(TargetData), ASC->ScopedPredictionKey);

			SharedInitAndApply();
			Super::Activate();
		}
		else
		{
			// Register with the TargetData callbacks if we are expecting client to send them
			const FGameplayAbilitySpecHandle	SpecHandle = GetAbilitySpecHandle();

			//Since multifire is supported, we still need to hook up the callbacks
			CallBackHandle = ASC->SetReplicatedTargetDataDelegateEx(SpecHandle, NetSyncGuid, FAbilityTargetDataSetDelegate::FDelegate::CreateUObject(this, &ThisClass::OnTargetDataReplicatedCallback))
			;
			if (!ASC->CallReplicatedTargetDataDelegatesIfSetEx(SpecHandle, NetSyncGuid))
			{
				SetWaitingOnRemotePlayerData();
			}
		}
	}
}

void UWKAbilityTask_PlayMontageWithRootMotionSource::InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent)
{
	Super::InitSimulatedTask(InGameplayTasksComponent);
}

void UWKAbilityTask_PlayMontageWithRootMotionSource::OnDestroy(bool bInOwnerFinished)
{
	if (MovementComponent && !bContinueRootMotionWhenMontageStop && bStopWhenAbilityEnds)
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}
	
	//主控端目标预测功能
	if (IsLocallyControlled())
	{
		// if (IsValidCreature(GetAvatarCreature()))
		// {
		// 	GetAvatarCreature()->K2_GetDSAbilitySystemComponent_Implementation()->ClearMutableDetectRule(this);
		// }
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UWKAbilityTask_PlayMontageWithRootMotionSource::PreDestroyFromReplication()
{
	if (!bIsSimulating)
	{
		if (AActor* MyActor = GetAvatarActor())
		{
			MyActor->ForceNetUpdate();
		}
	}
	EndTask();
	Super::PreDestroyFromReplication();
}

void UWKAbilityTask_PlayMontageWithRootMotionSource::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	FGameplayAbilityTargetDataHandle CopiedData = Data;  // 拷贝数据指针，因后续流程会清理数据

	if (UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(AbilitySystemComponent.Get()))
	{
		const FGameplayAbilitySpecHandle	SpecHandle = GetAbilitySpecHandle();
		ASC->RemoveReplicatedTargetDataDelegateEx(SpecHandle, NetSyncGuid, CallBackHandle);
	}
	ClearWaitingOnRemotePlayerData();

	const FGameplayAbilityTargetData* TargetData = CopiedData.Get(0);
	check(TargetData);

	//DSLog(LogTemp,Warning, TEXT("OnTargetDataReplicatedCallback %s |%d |%.2f"),*MontageToPlay->GetName(),SectionIndex,AbilitySystemComponent->GetWorld()->GetTimeSeconds());

	{
		const FGameplayAbilityTargetData_PlayMontage* PlayMontageData = (FGameplayAbilityTargetData_PlayMontage*)TargetData;
		
		ActorRotator = PlayMontageData->ActorRotator;
		MARK_PROPERTY_DIRTY_FROM_NAME(UWKAbilityTask_PlayMontageWithRootMotionSource, ActorRotator, this);
		
		ControlRotator = PlayMontageData->ControlRotator;
		MARK_PROPERTY_DIRTY_FROM_NAME(UWKAbilityTask_PlayMontageWithRootMotionSource, ControlRotator, this);
		
		CharacterMoveInput = PlayMontageData->MoveInPut;
		MARK_PROPERTY_DIRTY_FROM_NAME(UWKAbilityTask_PlayMontageWithRootMotionSource, CharacterMoveInput, this);
		
		PlayerTarget = PlayMontageData->TargetActor.Get();
		MARK_PROPERTY_DIRTY_FROM_NAME(UWKAbilityTask_PlayMontageWithRootMotionSource, PlayerTarget, this);

		OverrideWarpingDistance = PlayMontageData->OverrideWarpingDistance;
		MARK_PROPERTY_DIRTY_FROM_NAME(UWKAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingDistance, this);
		
		OverrideWarpingDirection = PlayMontageData->OverrideWarpingDirection;
		MARK_PROPERTY_DIRTY_FROM_NAME(UWKAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingDirection, this);
		
		CustomTargetLocation = PlayMontageData->CustomTargetLocation;
		MARK_PROPERTY_DIRTY_FROM_NAME(UWKAbilityTask_PlayMontageWithRootMotionSource, CustomTargetLocation, this);

		OverrideWarpingRotation = PlayMontageData->OverrideWarpingRotation;
		MARK_PROPERTY_DIRTY_FROM_NAME(UWKAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingRotation, this);

		OverrideWarpingTargetActorParam = PlayMontageData->OverrideWarpingTargetActorParam;
		MARK_PROPERTY_DIRTY_FROM_NAME(UWKAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingTargetActorParam, this);

		if (GetAvatarActor())
		{
			//同步给模拟端
			GetAvatarActor()->ForceNetUpdate();
		}
		
		SharedInitAndApply();
		Super::Activate();
	}
}

void UWKAbilityTask_PlayMontageWithRootMotionSource::OnRep_ActorRotator()
{
}

void UWKAbilityTask_PlayMontageWithRootMotionSource::SharedInitAndApply()
{
	if (!IsValid(MontageToPlay))
	{
		return;
	}

	//转向主控端朝向，避免播放蒙太奇时方向不同步导致rootMotion模拟位移差距大而拉回
	if (GetAvatarActor())
	{
		GetAvatarActor()->SetActorRotation(ActorRotator);
	}
	
	HandleRootMotionWarping();
}

const bool UWKAbilityTask_PlayMontageWithRootMotionSource::NeedReplicateAutonomousClientParamToServer()
{
	if (AWKCharacterBase* Character = Cast<AWKCharacterBase>(GetAvatarActor()))
	{
		if (Character->GetNetMode() == NM_Standalone)
		{
			return false;
		}
		
		if (!Character->IsPlayerControlled())
		{
			return false;
		}

		if (Ability.Get() && (Ability->GetNetExecutionPolicy()==EGameplayAbilityNetExecutionPolicy::Type::LocalOnly
			|| Ability->GetNetExecutionPolicy()==EGameplayAbilityNetExecutionPolicy::Type::ServerOnly))
		{
			return false;
		}

		// TArray<const FAnimNotifyEvent*> MatchTargetNotifyEvents = UDSAnimationFunctionLibrary::GetNotifyEventsFromAnimMontage(MontageToPlay, UDSAnimNotifyState_RootMotionWarping::StaticClass(), SectionIndex);
		// if (!MatchTargetNotifyEvents.IsEmpty())
		// {
		// 	for (int i = 0; i < MatchTargetNotifyEvents.Num(); i++)
		// 	{
		// 		if (const UWKAnimNotifyState_RootMotionWarping* RootMotionWarpingNotify = Cast<UWKAnimNotifyState_RootMotionWarping>(MatchTargetNotifyEvents[i]->NotifyStateClass))
		// 		{
		// 			if (RootMotionWarpingNotify->MotionWarpingType == EDSRootMotionWarpingType::WarpingWithTarget
		// 				|| (RootMotionWarpingNotify->MotionWarpingType == EDSRootMotionWarpingType::WarpingWithRotation && RootMotionWarpingNotify->WarpingRotation.RotationType!=	EDSRootMotionRotationType::BaseOnActor))
		// 				//|| RootMotionWarpingNotify->MotionWarpingType == EDSRootMotionWarpingType::WarpingWithTranslation)
		// 			{
		// 				return true;
		// 			}
		// 		}
		// 	}
		// }

		//尽量避免所有动画都RPC来同步参数，因为如果Gamplay连续播放两个动画，服务器收到RPC时序不能确保，导致动画播放顺序不对
		/*if (MontageToPlay->HasRootMotion())
		{
			return true;
		}*/
	}
	return false;
}

void UWKAbilityTask_PlayMontageWithRootMotionSource::PrepareReplicateParams()
{
	// if (AWKCharacterBase* Character = Cast<AWKCharacterBase>(GetAvatarActor()))
	// {
	// 	ActorRotator = Character->GetActorRotation();
	// 	MARK_PROPERTY_DIRTY_FROM_NAME(UWKAbilityTask_PlayMontageWithRootMotionSource, ActorRotator, this);
	//
	// 	TArray<float> OverrideWarpingDistances;
	// 	TArray<FVector_NetQuantize100> OverrideWarpingDirections;
	// 	TArray<FVector_NetQuantize100> CustomTargetLocations;
	// 	TArray<float> OverrideWarpingRotations;
	// 	FWKOverrideWarpingTargetActorParam OverrideWarpingTargetActorParams;
	// 	
	// 	TArray<const FAnimNotifyEvent*> MatchTargetNotifyEvents = UDSAnimationFunctionLibrary::GetNotifyEventsFromAnimMontage(MontageToPlay, UDSAnimNotifyState_RootMotionWarping::StaticClass(), SectionIndex);
	// 	if (!MatchTargetNotifyEvents.IsEmpty())
	// 	{
	// 		float SectionStart = 0;
	// 		float SectionEnd = MontageToPlay->GetPlayLength();
	// 		if (SectionIndex != INDEX_NONE)
	// 		{
	// 			MontageToPlay->GetSectionStartAndEndTime(SectionIndex, SectionStart, SectionEnd);
	// 		}
	// 		SectionStart += StartTime;
	//
	// 		//只获取一次TargetActor
	// 		bool bFindTargetActor =  false;
	// 		
	// 		for (int i = 0; i < MatchTargetNotifyEvents.Num(); i++)
	// 		{
	// 			const FAnimNotifyEvent* Event = MatchTargetNotifyEvents[i];
	// 			if (Event->GetEndTriggerTime() <= SectionStart || Event->GetTriggerTime() >= SectionEnd)
	// 				continue;
	// 			
	// 			if (const UWKAnimNotifyState_RootMotionWarping* RootMotionWarpingNotify = Cast<UWKAnimNotifyState_RootMotionWarping>(MatchTargetNotifyEvents[i]->NotifyStateClass))
	// 			{
	// 				if (RootMotionWarpingNotify->MotionWarpingType == EDSRootMotionWarpingType::WarpingWithTarget)
	// 				{
	// 					if (IsValid(RootMotionWarpingNotify->WarpingTargetActor.TargetCatchRule))
	// 					{
	// 						if(!bFindTargetActor)
	// 						{
	// 							//应用特异性位移增加锁敌距离
	// 							UDSTargetCatchRule_PlayerCamera* TargetCatchRule = Cast<UDSTargetCatchRule_PlayerCamera>(RootMotionWarpingNotify->WarpingTargetActor.TargetCatchRule.Get());
	// 							if(IsValid(TargetCatchRule) && RootMotionWarpingNotify->WarpingTargetActor.bApplySpecificity)
	// 							{
	// 								TargetCatchRule->bApplySpecificity = true;
	// 							}
	// 							
	// 							PlayerTarget = RootMotionWarpingNotify->WarpingTargetActor.TargetCatchRule.Get()->TargetSearch(Character,Ability);
	// 							MARK_PROPERTY_DIRTY_FROM_NAME(UDSAbilityTask_PlayMontageWithRootMotionSource, PlayerTarget, this);
	// 							
	// 							bFindTargetActor = true;
	// 						}
	// 					}
	// 					if (IsValid(RootMotionWarpingNotify->WarpingTargetActor.OverrideWarpingTargetActor ))
	// 					{
	// 						OverrideWarpingTargetActorParams = RootMotionWarpingNotify->WarpingTargetActor.OverrideWarpingTargetActor.Get()->GetOverrideWarpingTargetParam(Character,Ability);
	// 					}
	// 				}
	//
	// 				if (RootMotionWarpingNotify->MotionWarpingType == EDSRootMotionWarpingType::WarpingWithRotation)
	// 				{
	// 					switch(RootMotionWarpingNotify->WarpingRotation.RotationType)
	// 					{
	// 					case EDSRootMotionRotationType::BaseOnActor:
	// 						if (IsValid(RootMotionWarpingNotify->WarpingRotation.OverrideWarpingRotation))
	// 						{
	// 							OverrideWarpingRotations.Add(RootMotionWarpingNotify->WarpingRotation.OverrideWarpingRotation.Get()->GetOverrideWarpingRotation(Character,Ability));
	// 						}
	// 						break;
	// 					case EDSRootMotionRotationType::TurnToTarget:
	// 						if (IsValid(RootMotionWarpingNotify->WarpingRotation.TargetCatchRule))
	// 						{
	// 							if(!bFindTargetActor)
	// 							{
	// 								PlayerTarget = RootMotionWarpingNotify->WarpingRotation.TargetCatchRule.Get()->TargetSearch(Character,Ability);
	// 								MARK_PROPERTY_DIRTY_FROM_NAME(UDSAbilityTask_PlayMontageWithRootMotionSource, PlayerTarget, this);
	//
	// 								bFindTargetActor = true;
	// 							}
	// 						}
	// 						break;
	// 					case EDSRootMotionRotationType::BaseOnController:
	// 						ControlRotator =  Character->GetControlRotation();
	// 						MARK_PROPERTY_DIRTY_FROM_NAME(UDSAbilityTask_PlayMontageWithRootMotionSource, ControlRotator, this);
	//
	// 						break;
	// 					case EDSRootMotionRotationType::BaseOnInput:
	// 						{
	// 							ControlRotator =  Character->GetControlRotation();
	// 							MARK_PROPERTY_DIRTY_FROM_NAME(UDSAbilityTask_PlayMontageWithRootMotionSource, ControlRotator, this);
	// 							ADSCharacter* PlayerCharacter = Cast<ADSCharacter>(Character);
	// 							CharacterMoveInput = FVector(PlayerCharacter->GetMoveInput().X,PlayerCharacter->GetMoveInput().Y,0);
	// 							MARK_PROPERTY_DIRTY_FROM_NAME(UDSAbilityTask_PlayMontageWithRootMotionSource, CharacterMoveInput, this);
	// 						}
	// 						break;
	// 					default:
	// 						break;
	// 					}
	// 				}
	//
	// 				if (RootMotionWarpingNotify->MotionWarpingType == EDSRootMotionWarpingType::WarpingWithTranslation)
	// 				{
	// 					if (IsValid(RootMotionWarpingNotify->WarpingTranslation.OverrideWarpingDistance ))
	// 					{
	// 						OverrideWarpingDistances.Add(RootMotionWarpingNotify->WarpingTranslation.OverrideWarpingDistance.Get()->GetOverrideWarpingDistance(Character,Ability));
	// 					}
	//
	// 					if (IsValid(RootMotionWarpingNotify->WarpingTranslation.OverrideWarpingDirection))
	// 					{
	// 						OverrideWarpingDirections.Add(RootMotionWarpingNotify->WarpingTranslation.OverrideWarpingDirection.Get()->GetOverrideWarpingDirection(Character,Ability));
	// 					}
	// 				}
	//
	// 				if (RootMotionWarpingNotify->MotionWarpingType == EDSRootMotionWarpingType::WarpingWithLocation)
	// 				{
	// 					if (IsValid(RootMotionWarpingNotify->WarpingLocation.CustomTargetLocation))
	// 					{
	// 						if(RootMotionWarpingNotify->bConvertLocationToTranslation)
	// 						{
	// 							FVector TargetLocation = RootMotionWarpingNotify->WarpingLocation.CustomTargetLocation.Get()->GetTargetLocation(Character,Ability);
	// 							float Distance = FVector::Dist(Character->GetActorLocation(),TargetLocation);
	// 					
	// 							OverrideWarpingDistances.Add(Distance);
	// 							OverrideWarpingDirections.Add((TargetLocation - Character->GetActorLocation()).GetSafeNormal());
	// 						
	// 						}else
	// 						{
	// 							CustomTargetLocations.Add(RootMotionWarpingNotify->WarpingLocation.CustomTargetLocation.Get()->GetTargetLocation(Character,Ability));
	// 						}
	// 					}
	// 				}
	// 			}
	// 		}
	//
	// 		if (bFindTargetActor)
	// 		{
	// 			Character->GetWKAbilitySystemComponent()->ProcessRotatorControlToTarget(PlayerTarget);
	// 		}
	// 	}
	//
	// 	OverrideWarpingDistance = OverrideWarpingDistances;
	// 	MARK_PROPERTY_DIRTY_FROM_NAME(UDSAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingDistance, this);
	//
	// 	OverrideWarpingDirection = OverrideWarpingDirections;
	// 	MARK_PROPERTY_DIRTY_FROM_NAME(UDSAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingDirection, this);
	//
	// 	CustomTargetLocation = CustomTargetLocations;
	// 	MARK_PROPERTY_DIRTY_FROM_NAME(UDSAbilityTask_PlayMontageWithRootMotionSource, CustomTargetLocation, this);
	//
	// 	OverrideWarpingRotation = OverrideWarpingRotations;
	// 	MARK_PROPERTY_DIRTY_FROM_NAME(UDSAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingRotation, this);
	//
	// 	OverrideWarpingTargetActorParam = OverrideWarpingTargetActorParams;
	// 	MARK_PROPERTY_DIRTY_FROM_NAME(UDSAbilityTask_PlayMontageWithRootMotionSource, OverrideWarpingTargetActorParam, this);
	// }
}

bool UWKAbilityTask_PlayMontageWithRootMotionSource::HandleRootMotionWarping()
{
	return true;
}

void UWKAbilityTask_PlayMontageWithRootMotionSource::ArrangeNotifySection(
	TArray<FRootMotionWarpingSection_Translation>& WarpingSections_Translations,
	TArray<FRootMotionWarpingSection_Rotation>& WarpingSections_Rotations,
	TArray<FRootMotionWarpingSection_TargetActor>& WarpingSections_TargetActors,
	TArray<FRootMotionWarpingSection_Location>& WarpingSections_Locations)
{
}
