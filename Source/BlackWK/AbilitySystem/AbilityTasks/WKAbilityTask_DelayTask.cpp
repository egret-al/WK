// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilityTask_DelayTask.h"

UWKAbilityTask_DelayTask::UWKAbilityTask_DelayTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bAllowRepeated(false)
	, DelayTime(0.f)
	, CurrentTime(0.f)
{
	bTickingTask = true;
}

UWKAbilityTask_DelayTask* UWKAbilityTask_DelayTask::DelayTask(UGameplayAbility* OwningAbility, const float DelayTime, bool bRepeated)
{
	if (UWKAbilityTask_DelayTask* NewTask = NewAbilityTask<UWKAbilityTask_DelayTask>(OwningAbility))
	{
		NewTask->bAllowRepeated = bRepeated;
		NewTask->DelayTime = DelayTime;

		return NewTask;
	}
	return nullptr;
}

void UWKAbilityTask_DelayTask::Activate()
{
	Super::Activate();
}

void UWKAbilityTask_DelayTask::TickTask(float DeltaTime)
{
	CurrentTime += DeltaTime;
	if (CurrentTime >= DelayTime)
	{
		CurrentTime = 0.f;
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnDelayDelegate.Broadcast();
		}
		
		if (!bAllowRepeated)
		{
			EndTask();
		}
	}
}
