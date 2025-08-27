// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilitySystemComponent.h"

#include "WKGameplayTags.h"
#include "Abilities/WKGameplayAbility.h"
#include "BlackWK/Animation/WKAnimInstanceBase.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "DataAssets/WKGameplayAbilityDataAsset.h"

UWKAbilitySystemComponent::UWKAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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

void UWKAbilitySystemComponent::ProcessAbilityInput()
{
	if (HasMatchingGameplayTag(WKGameplayTags::InputTag_Gameplay_AbilityInputBlocked))
	{
		ClearAbilityInput();
		return;
	}

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// 处理所有输入持续激活的GA
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			const UWKGameplayAbility* AbilityCDO = Cast<UWKGameplayAbility>(AbilitySpec->Ability);
			if (AbilityCDO && AbilityCDO->GetActivationPolicy() == EWKAbilityActivationPolicy::WhileInputActive)
			{
				AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
			}
		}
	}

	// 处理所有在按下这一帧的GA
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UWKGameplayAbility* AbilityCDO = Cast<UWKGameplayAbility>(AbilitySpec->Ability);
					if (AbilityCDO && AbilityCDO->GetActivationPolicy() == EWKAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	// 处理所有松开按键时触发的GA
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UWKAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
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
			//AbilityInputListenerHandles->Remove(ListenerHandle);
			return;
		}
	}
}

void UWKAbilitySystemComponent::AbilityLocalInputPressed(int32 InputID)
{
	Super::AbilityLocalInputPressed(InputID);

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

	// 检测是否有其他ability关注了该事件，使用GameCustom1自主监听Pressed事件
	TArray<FAbilityInputListenerHandle>* ListenerHandles = InputListeners.Find(InputID);
	
	if (ListenerHandles&&!ListenerHandles->IsEmpty())
	{
		TArray<FAbilityInputListenerHandle> ListenerHandlesCopy = *ListenerHandles;
		for (FAbilityInputListenerHandle&  Handle : ListenerHandlesCopy)
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
