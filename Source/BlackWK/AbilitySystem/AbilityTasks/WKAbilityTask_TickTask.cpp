// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilityTask_TickTask.h"

UWKAbilityTask_TickTask::UWKAbilityTask_TickTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TickTimeInterval(0.1f)
	, bEnableTickInterval(true)
	, LastTickTime(0)
{
	bTickingTask = true;
}

UWKAbilityTask_TickTask* UWKAbilityTask_TickTask::TickTask(UGameplayAbility* OwningAbility, const float TickFrequency)
{
	if (UWKAbilityTask_TickTask* MyObj = NewAbilityTask<UWKAbilityTask_TickTask>(OwningAbility))
	{
		if (TickFrequency > 0)
		{
			MyObj->TickTimeInterval = 1.f / TickFrequency;
			MyObj->bEnableTickInterval = true;
		}
		else if(FMath::IsNearlyEqual(TickFrequency, -1.f))
		{
			MyObj->bEnableTickInterval = false;
		}
		
		return MyObj;
	}
	return nullptr;
}

void UWKAbilityTask_TickTask::Activate()
{
	Super::Activate();
}

void UWKAbilityTask_TickTask::TickTask(float DeltaTime)
{
	if (bEnableTickInterval)
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime > LastTickTime + TickTimeInterval)
		{
			LastTickTime = CurrentTime;
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnTickDelegate.Broadcast(TickTimeInterval);
			}
		}
	}
	else
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnTickDelegate.Broadcast(DeltaTime);
		}
	}
}
