// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilitySystemComponent.h"

#include "WKGameplayTags.h"
#include "Abilities/WKGameplayAbility.h"
#include "BlackWK/Animation/WKAnimInstanceBase.h"
#include "BlackWK/Character/WKAICharacterBase.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/Character/WKPlayerCharacterBase.h"
#include "BlackWK/Data/WKGameplaySettings.h"
#include "BlackWK/Player/WKPlayerController.h"
#include "DataAssets/WKDataTypes.h"
#include "DataAssets/WKGameplayAbilityDataAsset.h"
#include "Kismet/KismetMathLibrary.h"

UWKAbilitySystemComponent::UWKAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UWKAbilitySystemComponent::OnRegister()
{
	Super::OnRegister();

	UWKGameplaySettings* GameplaySettings = UWKGameplaySettings::GetGameplaySettings();
	if (!GameplaySettings)
	{
		return;
	}

	UDataTable* SkillTable = GameplaySettings->SkillTable.LoadSynchronous();
	if (!SkillTable)
	{
		return;
	}

	SkillTableRowMap.Empty();
	TArray<FWKSkillTableRow*> SkillTableRows;
	SkillTable->GetAllRows<FWKSkillTableRow>(TEXT(""), SkillTableRows);
	for (FWKSkillTableRow* SkillTableRow : SkillTableRows)
	{
		SkillTableRowMap.Add(SkillTableRow->SkillID, SkillTableRow);
	}
}

void UWKAbilitySystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateRotatorControlToTarget(DeltaTime);
}

void UWKAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	// 获取之前设置的Pawn信息
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);
	
	// 判断是否有新的Pawn作为Avatar初始化
	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && InAvatarActor != ActorInfo->AvatarActor;
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bHasNewPawnAvatar)
	{
		// 通知所有的GA，有新的Pawn设置上了
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			UWKGameplayAbility* WKGameplayAbilityCDO = Cast<UWKGameplayAbility>(AbilitySpec.Ability);
			if (!WKGameplayAbilityCDO)
			{
				continue;
			}
			
			// 如果是不需要实例化的GA，通知CDO对象，否则通知实例GA
			if (WKGameplayAbilityCDO->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::NonInstanced)
			{
				WKGameplayAbilityCDO->OnPawnAvatarSet();
			}
			else
			{
				// 通知所有这个GA的实例
				TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
				for (UGameplayAbility* AbilityInstance : Instances)
				{
					if (UWKGameplayAbility* WKAbilityInstance = Cast<UWKGameplayAbility>(AbilityInstance))
					{
						WKAbilityInstance->OnPawnAvatarSet();
					}
				}
			}
		}

		// 告知AnimInstance
		if (UWKAnimInstanceBase* WKAnimInstance = Cast<UWKAnimInstanceBase>(ActorInfo->GetAnimInstance()))
		{
			WKAnimInstance->InitializeWithAbilitySystem(this);
		}
	}
}

void UWKAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, BlockTags, bExecuteCancelTags, CancelTags);
}

bool UWKAbilitySystemComponent::HasMatchingGameplayTagExactly(FGameplayTag TagToCheck) const
{
	return GameplayTagCountContainer.GetExplicitGameplayTags().HasTagExact(TagToCheck);
}

void UWKAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				AbilitiesToActivate.AddUnique(AbilitySpec.Handle);
				// InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				// InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}
}

void UWKAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UWKAbilitySystemComponent::AddGameplayTag(FGameplayTag AddToTag)
{
	AddLooseGameplayTag(AddToTag);
}

void UWKAbilitySystemComponent::RemoveGameplayTag(FGameplayTag RemoveTag)
{
	RemoveLooseGameplayTag(RemoveTag);
}

FSimpleMulticastDelegate& UWKAbilitySystemComponent::WKAbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::Type EventType, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey)
{
	FGameplayAbilitySpecHandleAndPredictionKey PredictionKey(AbilityHandle, AbilityOriginalPredictionKey);
	TSharedRef<FAbilityReplicatedDataCache> ReplicatedDataCache = AbilityTargetDataMap.FindOrAdd(PredictionKey);
	return ReplicatedDataCache->GenericEvents[EventType].Delegate;
}

void UWKAbilitySystemComponent::UpdateCurrentPriorityAbility(UWKGameplayAbility* RequestAbility, const FWKGameplayAbilityPriorityInfo& PriorityInfo)
{
	if (!RequestAbility)
	{
		return;
	}

	if (GetCurrentPriorityAbility() != RequestAbility)
	{
		return;
	}

	ApplyPriorityBlockAndCancel(RequestAbility, PriorityInfo, false);
}

UWKGameplayAbility* UWKAbilitySystemComponent::GetCurrentPriorityAbility() const
{
	return CurrentPriorityAbility;
}

FWKGameplayAbilityPriorityInfo UWKAbilitySystemComponent::GetCurrentPriorityInfo() const
{
	return CurrentPriorityInfo;
}

void UWKAbilitySystemComponent::ApplyPriorityBlockAndCancel(UWKGameplayAbility* RequestAbility, const FWKGameplayAbilityPriorityInfo& PriorityInfo, bool bEndAbility)
{
	if (!RequestAbility)
	{
		return;
	}
	
	// 如果GA配置了只用于检测能否激活，则不纳入系统管理
	if (!RequestAbility->HasPriority() || RequestAbility->CheckPriorityActivateOnly())
	{
		if(CurrentPriorityAbility && RequestAbility != CurrentPriorityAbility && !bEndAbility && Cast<UWKGameplayAbility>(RequestAbility)->AbilityDataAsset->PriorityInfo.bCancelLastActiveAbility == true)
		{
			CurrentPriorityAbility->AbilityInterruptedByPriority(RequestAbility);
			CancelAbilitySpec(*CurrentPriorityAbility->GetCurrentAbilitySpec(), RequestAbility);
		}
		return;
	}

	// 如果是EndAbility，直接清空优先级数据到初始状态
	if (bEndAbility)
	{
		// 保护，确保结束的GA与当前GA一致，不然就是有问题系统存在混乱
		if (CurrentPriorityAbility == RequestAbility)
		{
			CurrentPriorityInfo = FWKGameplayAbilityPriorityInfo();
			LastPriorityAbility = CurrentPriorityAbility;
			if (GetWorld())
			{
				LastPriorityAbilityTime = GetWorld()->GetTimeSeconds();
			}
			CurrentPriorityAbility = nullptr;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
				FString::Printf(TEXT("Priority GA ERROR (%s): 当前技能与取消技能不吻合.. 系统当前：%s --- 尝试取消：%s"),
					IsOwnerActorAuthoritative() ? TEXT("Server") : TEXT("Client"),
					CurrentPriorityAbility == nullptr ? TEXT("nullptr") : *CurrentPriorityAbility->GetName(),
					*RequestAbility->GetName())
			);
		}
	}
	else
	{
		// 之前有PriorityGA被激活，先Cancel
		if (CurrentPriorityAbility != nullptr)
		{
			if (GetWorld())
			{
				LastPriorityAbilityTime = GetWorld()->GetTimeSeconds();
			}
			// 如果等于自己，则为动态修改优先级，不需要取消
			if (CurrentPriorityAbility != RequestAbility && Cast<UWKGameplayAbility>(RequestAbility)->HasPriority())
			{
				if (CurrentPriorityAbility->CheckPriorityActivateInterrupt(RequestAbility->AbilityDataAsset))
				{
					if (RequestAbility->bServerCheckCanActive)
					{
						//DSLog(DSLogGAS, Log, TEXT("-- JLSS %s -- Cancel call !! ----- Request:%s ----- Target: %s"), IsOwnerActorAuthoritative() ? TEXT("Server") : TEXT("Client"), *RequestAbility->GetName(), *CurrentPriorityAbility->GetName());
						float RequestPriority = Cast<UWKGameplayAbility>(RequestAbility)->AbilityDataAsset->PriorityInfo.Priority;
						//ensureAlways(CurrentPriorityInfo.Priority < RequestPriority, TEXT("旧的优先级更高的情况绝对不该走到这。。。。。"));

						if (CurrentPriorityInfo.Priority >= RequestPriority)
						{
							// DSLog(DSLogGAS, Warning, TEXT("服务器优先执行的GA打断了Client本地刚执行的GA"));
						}
					}
					// DSLog(DSLogGAS, Verbose, TEXT("%s: PriorityCancel: CurrentPriorityAbility: %s, RequestAbility: %s "), IsOwnerActorAuthoritative() ? TEXT("Server") : TEXT("Client"), *CurrentPriorityAbility->GetName(), *RequestAbility->GetName());
					LastPriorityAbility = CurrentPriorityAbility;
					// if(CVarShowPriorityInterrupt.GetValueOnAnyThread() > 0)
					// {
					// 	DSLog(DSLogGAS, Log, TEXT("GA :%s interrupt GA :%s"),*RequestAbility->GetName(), *CurrentPriorityAbility->GetName());
					// }
					// ShowInterruptedUI(RequestAbility, CurrentPriorityAbility);
					CurrentPriorityAbility->AbilityInterruptedByPriority(RequestAbility);
					CancelAbilitySpec(*CurrentPriorityAbility->GetCurrentAbilitySpec(), RequestAbility);
				}
			}
		}

		// 优先级修改广播
		if (OnAbilityPriorityUpdateDelegate.IsBound())
		{
			OnAbilityPriorityUpdateDelegate.Broadcast(CurrentPriorityInfo.Priority, PriorityInfo.Priority);
		}

		// 最后更新数据
		CurrentPriorityInfo = PriorityInfo;

		CurrentPriorityAbility = RequestAbility;
	}
}

void UWKAbilitySystemComponent::ClientSetReplicatedTargetDataEx_Implementation(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid, const FGameplayAbilityTargetDataHandle& TargetDataHandle, FPredictionKey CurrentPredictionKey)
{
	// 使用 Server 接口相同实现
	ServerSetReplicatedTargetDataEx_Implementation(AbilityHandle, Guid, TargetDataHandle, CurrentPredictionKey);
}

bool UWKAbilitySystemComponent::ClientSetReplicatedTargetDataEx_Validate(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid, const FGameplayAbilityTargetDataHandle& TargetDataHandle, FPredictionKey CurrentPredictionKey)
{
	return ServerSetReplicatedTargetDataEx_Validate(AbilityHandle, Guid, TargetDataHandle, CurrentPredictionKey);
}

void UWKAbilitySystemComponent::ServerSetReplicatedTargetDataEx_Implementation(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid, const FGameplayAbilityTargetDataHandle& TargetDataHandle, FPredictionKey CurrentPredictionKey)
{
	// 解决数据错乱问题. 父类方法中 AbilityTargetDataMap 的 Key 是重复的.
	//    原问题：同个GA中多处触发此方法，会导致 Find 到相同数据，并覆盖 TargetData。
	//		会在使用处进行非法数据转换。造成通知异常(可能内存强转错误)。

	// DSLog(DSLogGAS, Verbose, TEXT("ASC: %s ServerSetReplicatedTargetDataEx Handle:%s Guid:%s"), GetOwner()?*GetOwner()->GetName():TEXT("nullptr"), *AbilityHandle.ToString(), *Guid.ToString());

	// 异常数据提示
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
		if (Spec && Spec->Ability && !Spec->IsActive())
		{
			// 通常发生在Client的GA激活并发送了RPC，但Server技能激活失败。
			//		此时收到数据将会缓存在内存中，下一次的GA设置数据侦听会立即应用旧数据并回调。可能造成应用到逻辑的数据不正确。
			//		则：应使用'return'忽略本条数据
			// 也会发生在 Client GA已结束，但此处依然收到 Server 网络消息
			// DSLog(DSLogGAS, Warning, TEXT("Ability %s. 收到消息'Set ReplicatedTargetData' 但当前GA并未激活."), *Spec->Ability->GetName());
			return;
		}
	}

	FScopedPredictionWindow ScopedPrediction(this, CurrentPredictionKey);

	// 查找或添加，获取引用
	FWKAbilityReplicatedDataCache& ReplicatedData = AbilityTargetDataMapEx.FindOrAdd(FGameplayAbilityGuidKey(AbilityHandle, Guid));

	// 异常数据提示
	if (ReplicatedData.TargetData.Num() > 0)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
		if (Spec && Spec->Ability)
		{
			// DSLog(DSLogGAS, Error, TEXT("Ability %s. 覆盖了待处理的 replicated target data. "), *Spec->Ability->GetName());
		}
	}

	// 赋值并广播
	ReplicatedData.TargetData = TargetDataHandle;
	ReplicatedData.bTargetConfirmed = true;
	ReplicatedData.bTargetCancelled = false;
	ReplicatedData.PredictionKey = CurrentPredictionKey;

	// 创建一个副本。广播过程中的后续逻辑会修改'AbilityTargetDataMapEx'。造成'TargetSetDelegate'内存回收。
	FAbilityTargetDataSetDelegate DelegateCopy = ReplicatedData.TargetSetDelegate;
	DelegateCopy.Broadcast(TargetDataHandle, ReplicatedData.ApplicationTag);
}

bool UWKAbilitySystemComponent::ServerSetReplicatedTargetDataEx_Validate(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid, const FGameplayAbilityTargetDataHandle& TargetDataHandle, FPredictionKey CurrentPredictionKey)
{
	if (!Guid.IsValid())
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
		if (Spec && Spec->Ability)
		{
			// DSLog(DSLogGAS, Log, TEXT("Ability %s. ServerSetReplicatedTargetDataEx_Validate Invalid Guid."), *Spec->Ability->GetName());
		}
	}
	// check the data coming from the client to ensure it's valid
	for (const TSharedPtr<FGameplayAbilityTargetData>& TgtData : TargetDataHandle.Data)
	{
		if (!ensure(TgtData.IsValid()))
		{
			return false;
		}
	}
	return true;
}

FDelegateHandle UWKAbilitySystemComponent::SetReplicatedTargetDataDelegateEx(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid, FAbilityTargetDataSetDelegate::FDelegate&& InDelegate)
{
	// DSLog(DSLogGAS, Verbose, TEXT("ASC: %s SetReplicatedTargetDataDelegateEx Handle:%s Guid:%s"), GetOwner()?*GetOwner()->GetName():TEXT("nullptr"), *AbilityHandle.ToString(), *Guid.ToString());
	FWKAbilityReplicatedDataCache& ReplicatedData = AbilityTargetDataMapEx.FindOrAdd(FGameplayAbilityGuidKey(AbilityHandle, Guid));

	ReplicatedData.DelegateCount++;
	// 如果数据已设置，则打印Error日志。逻辑上不应存在相同GUID同时多次触发
	if (ReplicatedData.DelegateCount > 1)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
		// DSLog(DSLogGAS, Error, TEXT("Ability %s. ReplicatedData 被多次设置回调(当前第 %d 次). 只应存在1次。"), (Spec && Spec->Ability) ? *Spec->Ability->GetName() : TEXT("Unknown"), ReplicatedData.DelegateCount);
	}

	return ReplicatedData.TargetSetDelegate.Add(InDelegate);
}

void UWKAbilitySystemComponent::RemoveReplicatedTargetDataDelegateEx(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid, FDelegateHandle DelegateHandle)
{
	if (const auto& CachedData = AbilityTargetDataMapEx.Find(FGameplayAbilityGuidKey(AbilityHandle, Guid)))
	{
		// DSLog(DSLogGAS, Verbose, TEXT("ASC: %s RemoveReplicatedTargetDataDelegateEx Handle:%s Guid:%s"), GetOwner()?*GetOwner()->GetName():TEXT("nullptr"), *AbilityHandle.ToString(), *Guid.ToString());
		CachedData->TargetSetDelegate.Remove(DelegateHandle);

		CachedData->DelegateCount--;
	}
}

bool UWKAbilitySystemComponent::CallReplicatedTargetDataDelegatesIfSetEx(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid)
{
	bool CalledDelegate = false;
	if (const FWKAbilityReplicatedDataCache* CachedData = AbilityTargetDataMapEx.Find(FGameplayAbilityGuidKey(AbilityHandle, Guid)))
	{
		// Use prediction key that was sent to us
		FScopedPredictionWindow ScopedWindow(this, CachedData->PredictionKey, false);

		if (CachedData->bTargetConfirmed)
		{
			// DSLog(DSLogGAS, Verbose, TEXT("ASC: %s CallReplicatedTargetDataDelegatesIfSetEx Handle:%s Guid:%s"), GetOwner()?*GetOwner()->GetName():TEXT("nullptr"), *AbilityHandle.ToString(), *Guid.ToString());

			if (CachedData->DelegateCount > 1)
			{
				FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
				// DSLog(DSLogGAS, Error, TEXT("Ability %s. ReplicatedData 存在多个回调(%d). 只应存在1次。"), (Spec && Spec->Ability) ? *Spec->Ability->GetName() : TEXT("Unknown"), CachedData->DelegateCount);
			}

			// 创建一个副本。广播过程中的后续逻辑会修改'AbilityTargetDataMapEx'。
			const FGameplayAbilityTargetDataHandle TargetDataCopy = CachedData->TargetData;
			const FAbilityTargetDataSetDelegate DelegateCopy = CachedData->TargetSetDelegate;
			DelegateCopy.Broadcast(TargetDataCopy, CachedData->ApplicationTag);
			CalledDelegate = true;
		}
		else if (CachedData->bTargetCancelled)
		{
			CachedData->TargetCancelledDelegate.Broadcast();
			CalledDelegate = true;
		}
	}

	return CalledDelegate;
}

void UWKAbilitySystemComponent::ClearAbilityReplicatedDataCacheEx(FGameplayAbilitySpecHandle Handle, const FGuid& Guid)
{
	// DSLog(DSLogGAS, Verbose, TEXT("ASC: %s ClearAbilityReplicatedDataCacheEx: Handle:%s GUID:%s"), GetOwner()?*GetOwner()->GetName():TEXT("nullptr"), *Handle.ToString(), *Guid.ToString());
#if !UE_BUILD_SHIPPING
	// 额外清理一次数据。预防外部逻辑错误使用旧的Found的指针。
	if (const auto CachedData = AbilityTargetDataMapEx.Find(FGameplayAbilityGuidKey(Handle, Guid)))
	{
		CachedData->TargetData.Clear();
	}
#endif
	AbilityTargetDataMapEx.Remove(FGameplayAbilityGuidKey(Handle, Guid));
}

void UWKAbilitySystemComponent::ClearAbilityReplicatedDataCacheEx(FGameplayAbilitySpecHandle Handle)
{
	if (AbilityTargetDataMapEx.Num() == 0)
	{
		return;
	}
	// DSLog(DSLogGAS, Verbose, TEXT("ASC: %s ClearAbilityReplicatedDataCacheEx MapSize:%d Handle:%s"), GetOwner()?*GetOwner()->GetName():TEXT("nullptr"), AbilityTargetDataMapEx.Num(), *Handle.ToString());
	for (auto It = AbilityTargetDataMapEx.CreateIterator(); It; ++It)
	{
		if (It.Key().AbilityHandle == Handle)
		{
			It.Value().TargetData.Clear();
			It.RemoveCurrent();  // 安全删除当前元素
		}
	}
}

void UWKAbilitySystemComponent::ProcessRotatorControlToTarget(AActor* Target)
{
	if (!RotatorControlToTarget)
	{
		return;
	}
	
	if (const AWKCharacterBase* Owner = Cast<AWKCharacterBase>(GetAvatarActor()); !Owner->IsLocallyControlled())
	{
		return;
	}
	
	if (IsValid(Target))
	{
		TurnToTarget = Target;
		//SpringState.Reset();
		RotatorControlToTargetTimer = 0.f;
	}
}

void UWKAbilitySystemComponent::UpdateRotatorControlToTarget(float DeltaTime)
{
	if (IsValid(TurnToTarget.Get()) && IsValid(GetOwner()))
	{
		if (RotatorControlToTargetTimer < RotatorDuration)
		{
			RotatorControlToTargetTimer += DeltaTime;

			// if (const AWKPlayerCharacterBase* Owner = Cast<AWKPlayerCharacterBase>(GetAvatarActor()); IsValid(Owner))
			// {
			// 	if (IsValid(Owner->GetPlayerController()) && !Owner->GetPlayerController()->bHasRotationInputThisFrame)
			// 	{
			// 		//UE_LOG(DSLogGAS, Warning, TEXT("没有转向输入，自动转向"));
			// 		FRotator CharacterRotator = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(),TurnToTarget->GetActorLocation());
			// 		FVector StartLocation = Owner->GetActorLocation() + CharacterRotator.RotateVector(Cast<UDSCameraMode_ThirdPerson>(Owner->GetDSCameraComponent()->DefaultCameraMode.GetDefaultObject())->CameraLocationOffset + Owner->GetDSCameraComponent()->CameraLocationOffsetAdditive);
			// 		FRotator CurrentRotator = Owner->GetController()->GetControlRotation();
			// 		FRotator TargetRotator = UKismetMathLibrary::FindLookAtRotation(StartLocation,TurnToTarget->GetActorLocation());
			// 		
			// 		FRotator ResultRotator = FMath::RInterpTo( CurrentRotator,TargetRotator, DeltaTime, InterpSpeed);
			// 		Owner->GetController()->SetControlRotation(ResultRotator);
			// 		
			// 		//FQuat TargetRotator = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(),TurnToTarget->GetActorLocation()).Quaternion(); 
			// 		//FQuat CurrentRotator = Owner->GetController()->GetControlRotation().Quaternion();	
			// 		//FQuat ResultRotator = UKismetMathLibrary::QuaternionSpringInterp(CurrentRotator, TargetRotator, SpringState, Stiffness, CriticalDamping,DeltaTime, Mass, TargetVelocityAmount);
			// 		//Owner->GetController()->SetControlRotation(ResultRotator.Rotator());
			// 	}
			// 	else
			// 	{
			// 		//UE_LOG(DSLogGAS, Warning, TEXT("有转向输入，停止自动转向"));
			// 		//有输入了，直接打断本次转向
			// 		TurnToTarget.Reset();
			// 	}
			// }
		}
		else
		{
			TurnToTarget.Reset();
		}
	}
}

bool UWKAbilitySystemComponent::GetSkillRowByID(const int32 SkillID, FWKSkillTableRow& OutRow) const
{
	if (FWKSkillTableRow* const* FindRow = SkillTableRowMap.Find(SkillID))
	{
		OutRow = **FindRow;
		return true;
	}
	return false;
}

TArray<FWKSkillTableRow> UWKAbilitySystemComponent::GetSkillTableRows() const
{
	TArray<FWKSkillTableRow> Rows;
	for (auto SkillTableRow : SkillTableRowMap)
	{
		Rows.Add(*SkillTableRow.Value);
	}
	return Rows;
}

void UWKAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK()
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (const UWKGameplayAbility* AbilityCDO = Cast<UWKGameplayAbility>(AbilitySpec.Ability))
		{
			AbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
		}
	}
}

void UWKAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// 不直接使用UGameplayAbility::bReplicateInputDirectly，改用同步事件的方式替换以让WaitInputPress Task正常使用
	if (Spec.IsActive())
	{
		// 调用输入按下的事件
		for (UGameplayAbility* AbilityInstance : Spec.GetAbilityInstances())
		{
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
		}
	}
}

void UWKAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		for (UGameplayAbility* AbilityInstance : Spec.GetAbilityInstances())
		{
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
		}
	}
}

void UWKAbilitySystemComponent::AbilityListenWithInput(int32 InputID, FGameplayAbilitySpecHandle AbilityHandle, EAbilityGenericReplicatedEvent::Type ListenType)
{
	TArray<FAbilityInputListenerHandle>& AbilityInputListenerHandles = InputListeners.FindOrAdd(InputID);
	for (auto& ListenerHandle : AbilityInputListenerHandles)
	{
		if (ListenerHandle.AbilityHandle == AbilityHandle)
		{
			ListenerHandle.bListen[ListenType] = true;
			return;
		}
	}
	FAbilityInputListenerHandle ListenerHandle = FAbilityInputListenerHandle(AbilityHandle);
	ListenerHandle.bListen[ListenType] = true;
	AbilityInputListenerHandles.Add(ListenerHandle);
}

void UWKAbilitySystemComponent::AbilityCancelListenWithInput(int32 InputID, FGameplayAbilitySpecHandle AbilityHandle, EAbilityGenericReplicatedEvent::Type ListenType)
{
	TArray<FAbilityInputListenerHandle>& AbilityInputListenerHandles = InputListeners.FindOrAdd(InputID);
	for (auto& ListenerHandle : AbilityInputListenerHandles)
	{
		if (ListenerHandle.AbilityHandle == AbilityHandle)
		{
			ListenerHandle.bListen[ListenType] = false;
			return;
		}
	}
}

void UWKAbilitySystemComponent::AbilityLocalInputPressed(int32 InputID)
{
	// Super::AbilityLocalInputPressed(InputID);

	OnAbilityInputPressedDelegate.Broadcast(InputID);

	// Consume the input if this InputID is overloaded with GenericConfirm/Cancel and the GenericConfim/Cancel callback is bound
	if (IsGenericConfirmInputBound(InputID))
	{
		LocalInputConfirm();
		return;
	}
	
	if (IsGenericCancelInputBound(InputID))
	{
		LocalInputCancel();
		return;
	}
	
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID != InputID)
		{
			continue;
		}
	
		// 检测是否已经激活
		if (Spec.IsActive())
		{
			// RPC Batching
			if (UWKGameplayAbility* WKAbility = Cast<UWKGameplayAbility>(Spec.Ability))
			{
				if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
				{
					ServerSetInputPressed(Spec.Handle);
				}
	
				AbilitySpecInputPressed(Spec);
				// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.GetPrimaryInstance()->GetCurrentActivationInfo().GetActivationPredictionKey());
	
				if (WKAbility)
				{
					if (WKAbility->AbilityDataAsset && WKAbility->AbilityDataAsset->TryReactive)
					{
						TryActivateAbility(Spec.Handle);
					}
				}
			}
		}
		else
		{
			// Ability is not active, so try to activate it
			TryActivateAbility(Spec.Handle);
		}
	}

	// 查找关注这个输入ID的GA Handle
	TArray<FAbilityInputListenerHandle>* ListenerHandles = InputListeners.Find(InputID);
	
	if (ListenerHandles && !ListenerHandles->IsEmpty())
	{
		TArray<FAbilityInputListenerHandle> ListenerHandlesCopy = *ListenerHandles;
		for (FAbilityInputListenerHandle& Handle : ListenerHandlesCopy)
		{
			for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
			{
				if (Spec.Handle == Handle.AbilityHandle)
				{
					FPredictionKey CurrentPredictionKey = FPredictionKey();
					InvokeReplicatedEventWithPayload(EAbilityGenericReplicatedEvent::GameCustom5, Spec.Handle, Spec.GetPrimaryInstance()->GetCurrentActivationInfo().GetActivationPredictionKey(), CurrentPredictionKey, FVector(InputID, 0, 0));		
				}
			}
		}
	}
}

void UWKAbilitySystemComponent::AbilityLocalInputReleased(int32 InputID)
{
	OnAbilityInputReleaseDelegate.Broadcast(InputID);
	
	Super::AbilityLocalInputReleased(InputID);

	ABILITYLIST_SCOPE_LOCK();
	//检测是否有其他ability关注了该事件使用GameCustom2自主监听Released事件
	TArray<FAbilityInputListenerHandle>* ListenerHandles = InputListeners.Find(InputID);
	if (ListenerHandles && !ListenerHandles->IsEmpty())
	{
		TArray<FAbilityInputListenerHandle> ListenerHandlesCopy = *ListenerHandles;
		for (FAbilityInputListenerHandle& Handle : ListenerHandlesCopy)
		{
			for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
			{
				if (Spec.Handle == Handle.AbilityHandle)
				{
					FPredictionKey CurrentPredictionKey = FPredictionKey();
					InvokeReplicatedEventWithPayload(EAbilityGenericReplicatedEvent::GameCustom6, Spec.Handle, Spec.GetPrimaryInstance()->GetCurrentActivationInfo().GetActivationPredictionKey(),CurrentPredictionKey, FVector(InputID, 0, 0));
				}
			}
		}
	}
}
