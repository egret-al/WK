// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/DataAssets/WKGameplayAbilityDataAsset.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/Player/WKPlayerState.h"

UWKGameplayAbility::UWKGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UWKGameplayAbility::AbilityInterruptedByPriority(UWKGameplayAbility* InterruptAbility)
{
	K2_AbilityInterruptedByPriority(InterruptAbility);
}

bool UWKGameplayAbility::CheckPriorityActivateInterrupt_Implementation(const UWKGameplayAbilityDataAsset* NewAbilityDataAsset) const
{
	return true;
}

void UWKGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& AbilitySpec) const
{
	const bool bIsPredicting = GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Predicting;

	if (ActorInfo && !AbilitySpec.IsActive() && !bIsPredicting && ActivationPolicy == EWKAbilityActivationPolicy::OnSpawn)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// 如果Avatar死亡，不再激活
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && AvatarActor->GetLifeSpan() <= 0.f)
		{
			// 本地是否执行
			const bool bIsLocalExecution = NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted || NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly;
			// 服务端是否执行
			const bool bIsServerExecution = NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly || NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated;

			// 本地客户端是否应该激活
			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			// 服务端是否应该激活
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

AWKCharacterBase* UWKGameplayAbility::GetWKCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AWKCharacterBase>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

UWKAbilitySystemComponent* UWKGameplayAbility::GetWKAbilitySystemComponentFromActorInfo() const
{
	return Cast<UWKAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

void UWKGameplayAbility::AddGameplayTag(FGameplayTag AddToTag)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(AddToTag);
	}
}

void UWKGameplayAbility::RemoveGameplayTag(FGameplayTag RemoveToTag)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(RemoveToTag);
	}
}

bool UWKGameplayAbility::HasPriority() const
{
	if (!AbilityDataAsset)
	{
		return false;
	}

	if (AbilityDataAsset->PriorityInfo.Priority < 0)
	{
		return false;
	}

	return true;
}

bool UWKGameplayAbility::CheckPriorityActivateOnly() const
{
	if (!AbilityDataAsset)
	{
		return false;
	}

	if (AbilityDataAsset->PriorityInfo.Priority < 0)
	{
		return false;
	}

	return AbilityDataAsset->PriorityInfo.bCheckPriorityActivateOnly;
}

void UWKGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	K2_OnAbilityAdded();
	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UWKGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	K2_OnAbilityRemoved();
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UWKGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	K2_InputPressed();
}

void UWKGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	K2_InputReleased();
}

bool UWKGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	AWKCharacterBase* OwnerAvatar = Cast<AWKCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!OwnerAvatar)
	{
		// 激活失败
		return false;
	}

	AWKPlayerState* PS = Cast<AWKPlayerState>(OwnerAvatar->GetPlayerState());
	if (!PS)
	{
		return false;
	}

	// 死亡不激活
	if (PS->GetHealth() <= 0.f)
	{
		return false;
	}

	// 检查Super方法
	bool bSuperResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	// 如果服务器不检测则直接成功
	UWKAbilitySystemComponent* ASC = OwnerAvatar->GetWKAbilitySystemComponent();
	if (!ASC)
	{
		return false;
	}
	
	if (ASC->GetOwner()->GetNetMode() != NM_Standalone && ASC->GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		if(!bServerCheckCanActive)
		{
			bSuperResult = true;
		}
	}
	
	// 如果Super验证通过，再检查其他
	if (bSuperResult && AbilityDataAsset)
	{
		// --------------- PriorityConfig ---------------
		// 不受优先级系统管理，直接成功
		if (!HasPriority())
		{
			return true;
		}
		
		if (!AbilityDataAsset->PriorityInfo.bForceActivateAbility)
		{
		    UWKGameplayAbilityDataAsset* NewAbilityDataAsset = ASC->GetCurrentPriorityAbility() ? ASC->GetCurrentPriorityAbility()->AbilityDataAsset : nullptr;
			
			// 如果优先级比当前低，直接return false
			if (!CheckPriorityActivateAbility(AbilityDataAsset->PriorityInfo.Priority, ASC->GetCurrentPriorityInfo().Priority, NewAbilityDataAsset))
			{
				if (OptionalRelevantTags)
				{
					// OptionalRelevantTags->AddTag(UDSGameplayTags::Get().GA_CancelledByPriority);
				}
				return false;
			}
		}

		// 通过，return true
		return true;
	}
	
	// if (OptionalRelevantTags)
	// {
	// 	OptionalRelevantTags->AddTag(UDSGameplayTags::Get().GA_CancelledBySuperCanActive);
	// }
	
	//UDSGameplayStatics::ShowFloatTips(this, FText(), ETipType::UnableRelease);
	if(ActorInfo == nullptr || !ActorInfo->AvatarActor.IsValid())
	{
		// DSLog(DSLogGAS_Ability, Error, TEXT("ActorInfo or AvatarActor is invalid"));
		return bSuperResult;
	}
	return bSuperResult;
}

void UWKGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UWKGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	// 技能激活时候把AbilityDataAsset中的默认优先级信息设置到ASC
	UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (IsValid(ASC) && IsValid(AbilityDataAsset))
	{
		if (AbilityDataAsset->PriorityInfo.Priority > 0 && !AbilityDataAsset->PriorityInfo.bCheckPriorityActivateOnly)
		{
			if (GetWorld())
			{
				ASC->SetCurrentPriorityAbilityTime(GetWorld()->GetTimeSeconds());
			}
		}
	}
	
	K2_PreActivate(TriggerEventData != nullptr ? *TriggerEventData : FGameplayEventData());
	
	// Call Super
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (IsValid(ASC))
	{
		// 只设置受打断系统管理的GA
		if (HasPriority())
		{
			ASC->ApplyPriorityBlockAndCancel(this, AbilityDataAsset->PriorityInfo, false);
		}
		
		// 最后处理特殊打断
		// if (AbilityDataAsset != nullptr)
		// {
		// 	ASC->ApplyInterruptBlockAndCancel(this, DataSource->InterruptInfo, true);
		// }
	}
}

void UWKGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	PreEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	// if (ActorInfo && IsValid(ActorInfo->AvatarActor.Get()))
	// {
	// 	if (AWKCharacterBase* OwnerAvatar = Cast<AWKCharacterBase>(ActorInfo->AvatarActor))
	// 	{
	// 		AWKPlayerState* OwnerPS = Cast<AWKPlayerState>(OwnerAvatar->GetPlayerState());
	// 	}
	// }
	// else
	// {
	// }
	
	if (TaskToBlockEndAbility.IsValid())
	{
		// 等待 Task 完成后，再 End Ability
		bWaitingTaskToEndAbility = true;
		EndInfoCache.Handle = Handle;
		EndInfoCache.ActivationInfo = ActivationInfo;
		EndInfoCache.bWasCancelled = bWasCancelled;
		EndInfoCache.bReplicateEndAbility = bReplicateEndAbility;
		return;
	}

	const bool IsEndValid = IsEndAbilityValid(Handle, ActorInfo);
	if (IsEndValid)
	{
		OnAbilityEnded(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

		// 清理Replicated相关缓存数据
		// if (UWKAbilitySystemComponent* ASC = GetWKAbilitySystemComponentFromActorInfo())
		// {
		// 	ASC->ClearAbilityReplicatedDataCacheEx(Handle);
		// }
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (IsEndValid)
	{
		OnPostAbilityEnded(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

void UWKGameplayAbility::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);

	// 被 Task 控制来 End Ability
	if (bWaitingTaskToEndAbility && TaskToBlockEndAbility.IsValid())
	{
		if (TaskToBlockEndAbility.Get() == &Task)
		{
			EndAbility(EndInfoCache.Handle, CurrentActorInfo, EndInfoCache.ActivationInfo, EndInfoCache.bReplicateEndAbility, EndInfoCache.bWasCancelled);
		}
	}
}

void UWKGameplayAbility::PreEndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	K2_PreEndAbility(ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UWKGameplayAbility::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
}

void UWKGameplayAbility::OnAbilityEnded(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
}

void UWKGameplayAbility::OnPostAbilityEnded(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (!ASC)
	{
		OnPostAbilityEndDelegate.Clear();
		return;
	}
	
	// 处理优先级
	if (HasPriority())
	{
		ASC->ApplyPriorityBlockAndCancel(this, FWKGameplayAbilityPriorityInfo(), true);
	}

	// if (AbilityDataAsset)
	// {
	// 	ASC->ApplyInterruptBlockAndCancel(this, FDSSpecialBlockAndInterruptInfo(), false);
	// }

	K2_PostEndAbilityEnded(bWasCancelled);

	OnPostAbilityEndDelegate.Broadcast(this);
	OnPostAbilityEndDelegate.Clear();
}

void UWKGameplayAbility::OnPawnAvatarSet()
{
	K2_OnPawnAvatarSet();
}

bool UWKGameplayAbility::SetAbilityTaskToBlockEndAbility(UAbilityTask* AbilityToSet)
{
	if (AbilityToSet)
	{
		TaskToBlockEndAbility = AbilityToSet;
		return true;
	}
	
	return false;
}

void UWKGameplayAbility::UnSetAbilityTaskToBlockEndAbility()
{
	TaskToBlockEndAbility.Reset();
}

bool UWKGameplayAbility::CheckPriorityActivateAbility_Implementation(float RequestPriority, float CurrentPriority, const UWKGameplayAbilityDataAsset* NewAbilityDataAsset) const
{
	return RequestPriority > CurrentPriority;
}
