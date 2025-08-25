// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilitySystemComponent.h"

#include "WKGameplayTags.h"
#include "Abilities/WKGameplayAbility.h"
#include "BlackWK/Animation/WKAnimInstanceBase.h"

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
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
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

void UWKAbilitySystemComponent::AbilityLocalInputPressed(int32 InputID)
{
	Super::AbilityLocalInputPressed(InputID);

	OnAbilityInputPressedDelegate.Broadcast(InputID);
}

void UWKAbilitySystemComponent::AbilityLocalInputReleased(int32 InputID)
{
	OnAbilityInputReleaseDelegate.Broadcast(InputID);
	
	Super::AbilityLocalInputReleased(InputID);
}
