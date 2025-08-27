// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilityTask_WaitInputRelease.h"

#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"

UWKAbilityTask_WaitInputRelease::UWKAbilityTask_WaitInputRelease(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, BindInputID(0)
	, StartTime(0.f)
	, bOnlyTriggerOnce(false)
{
}

UWKAbilityTask_WaitInputRelease* UWKAbilityTask_WaitInputRelease::WKWaitReleaseWithInput(UGameplayAbility* OwningAbility, EWKGameplayAbilityInputBinds InputID, bool bOnlyTriggerOnce)
{
	UWKAbilityTask_WaitInputRelease* Task = NewAbilityTask<UWKAbilityTask_WaitInputRelease>(OwningAbility);
	Task->BindInputID = static_cast<int>(InputID);
	Task->bOnlyTriggerOnce = bOnlyTriggerOnce;
	return Task;
}

void UWKAbilityTask_WaitInputRelease::Activate()
{
	StartTime = GetWorld()->GetTimeSeconds();
	if (Ability)
	{
		DelegateHandle = AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom6, GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnReleaseCallback);
		if (UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(AbilitySystemComponent))
		{
			ASC->AbilityListenWithInput(BindInputID, GetAbilitySpecHandle(), EAbilityGenericReplicatedEvent::GameCustom6);
		}
		if (IsForRemoteClient())
		{
			if (!AbilitySystemComponent->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom6, GetAbilitySpecHandle(), GetActivationPredictionKey()))
			{
				SetWaitingOnRemotePlayerData();
			}
		}
	}
}

void UWKAbilityTask_WaitInputRelease::OnDestroy(bool bInOwnerFinished)
{
	AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom6, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
	if (UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->AbilityCancelListenWithInput(BindInputID, GetAbilitySpecHandle(), EAbilityGenericReplicatedEvent::GameCustom6);
	}
	Super::OnDestroy(bInOwnerFinished);
}

void UWKAbilityTask_WaitInputRelease::OnReleaseCallback()
{
	const float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

	if (!Ability || !AbilitySystemComponent.IsValid())
	{
		return;
	}

	if (bOnlyTriggerOnce)
	{
		AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom6, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
		if (UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(AbilitySystemComponent))
		{
			ASC->AbilityCancelListenWithInput(BindInputID, GetAbilitySpecHandle(), EAbilityGenericReplicatedEvent::GameCustom6);
		}
	}
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());

	FAbilityReplicatedData ReplicatedData = AbilitySystemComponent->GetReplicatedDataOfGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom6, GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ReplicatedData.VectorPayload.X!=BindInputID)
	{
		return;
	}
	
	if (IsPredictingClient())
	{
		// Tell the server about this
		AbilitySystemComponent->ServerSetReplicatedEventWithPayload(EAbilityGenericReplicatedEvent::GameCustom6, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey,ReplicatedData.VectorPayload);
	}
	else
	{
		AbilitySystemComponent->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom6, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}
	
	// We are done. Kill us so we don't keep getting broadcast messages
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnRelease.Broadcast(ElapsedTime);
	}
	
	// 如果只激活一次则结束Task
	if (bOnlyTriggerOnce)
	{
		EndTask();
	}
}
