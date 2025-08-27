// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilityTask_WaitInputPressed.h"

#include "AbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"

UWKAbilityTask_WaitInputPressed::UWKAbilityTask_WaitInputPressed(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, BindInputID(0)
	, StartTime(0.f)
	, bOnlyTriggerOnce(false)
	, bWithPayload(false)
	, bPayloadIsVector(false)
{
}

UWKAbilityTask_WaitInputPressed* UWKAbilityTask_WaitInputPressed::WKWaitPressedWithInput(UGameplayAbility* OwningAbility, EWKGameplayAbilityInputBinds InputID, bool bOnlyTriggerOnce, bool bWithPayload, bool bPayloadIsVector, FVector Payload)
{
	UWKAbilityTask_WaitInputPressed* Task = NewAbilityTask<UWKAbilityTask_WaitInputPressed>(OwningAbility);
	Task->BindInputID = static_cast<int>(InputID);
	Task->bOnlyTriggerOnce = bOnlyTriggerOnce;
	Task->bWithPayload = bWithPayload;
	Task->bPayloadIsVector = bPayloadIsVector;
	Task->Payload = Payload;
	return Task;
}

void UWKAbilityTask_WaitInputPressed::Activate()
{
	StartTime = GetWorld()->GetTimeSeconds();
	if (Ability)
	{
		DelegateHandle = AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom5, GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnPressedCallback);
		if (UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(AbilitySystemComponent))
		{
			ASC->AbilityListenWithInput(BindInputID, GetAbilitySpecHandle(), EAbilityGenericReplicatedEvent::GameCustom5);
		}
		if (IsForRemoteClient())
		{
			if (!AbilitySystemComponent->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom5, GetAbilitySpecHandle(), GetActivationPredictionKey()))
			{
				SetWaitingOnRemotePlayerData();
			}
		}
	}
}

void UWKAbilityTask_WaitInputPressed::OnDestroy(bool bInOwnerFinished)
{
	AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom5, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
	if (UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->AbilityCancelListenWithInput(BindInputID, GetAbilitySpecHandle(), EAbilityGenericReplicatedEvent::GameCustom5);
	}
	Super::OnDestroy(bInOwnerFinished);
}

void UWKAbilityTask_WaitInputPressed::OnPressedCallback()
{
	const float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;
	if (!Ability || !AbilitySystemComponent.IsValid())
	{
		return;
	}

	if (bOnlyTriggerOnce)
	{
		AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom5, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
		if (UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(AbilitySystemComponent))
		{
			ASC->AbilityCancelListenWithInput(BindInputID, GetAbilitySpecHandle(), EAbilityGenericReplicatedEvent::GameCustom5);
		}
	}
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());

	int32 TempInputID = BindInputID;
	if (IsPredictingClient())
	{
		//如果不是同一按键返回
		FAbilityReplicatedData ReplicatedData = AbilitySystemComponent->GetReplicatedDataOfGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom5, GetAbilitySpecHandle(), GetActivationPredictionKey());
		if(BindInputID != ReplicatedData.VectorPayload.X)
		{
			return;
		}
		
		if (bWithPayload)
		{
			if (!bPayloadIsVector)
			{
				Payload.X = BindInputID;
			}
			else
			{
				TempInputID = ReplicatedData.VectorPayload.X;
			}
			
			AbilitySystemComponent->ServerSetReplicatedEventWithPayload(EAbilityGenericReplicatedEvent::GameCustom5, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey, Payload);
		}
		else
		{
			// Tell the server about this
			AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom5, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
	}
	else
	{
		FAbilityReplicatedData ReplicatedData = AbilitySystemComponent->GetReplicatedDataOfGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom5, GetAbilitySpecHandle(), GetActivationPredictionKey());
		if(bWithPayload && BindInputID != ReplicatedData.VectorPayload.X)
		{
			return;
		}
		Payload = ReplicatedData.VectorPayload;
		TempInputID = ReplicatedData.VectorPayload.X;
		AbilitySystemComponent->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom5, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}
	
	// We are done. Kill us so we don't keep getting broadcast messages
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (bWithPayload && TempInputID == BindInputID)
		{
			OnPressedWithPayload.Broadcast(Payload, TempInputID);
		}
		else
		{
			OnPressed.Broadcast(ElapsedTime);
		}
	}
	
	// 如果只激活一次则结束Task
	if (bOnlyTriggerOnce)
	{
		EndTask();
	}
}
