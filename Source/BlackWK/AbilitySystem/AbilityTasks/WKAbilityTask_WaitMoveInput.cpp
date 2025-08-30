// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilityTask_WaitMoveInput.h"

#include "BlackWK/Input/WKPlayerCharacterInputComponent.h"

UWKAbilityTask_WaitMoveInput::UWKAbilityTask_WaitMoveInput(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InputComponent(nullptr)
	, bOnlyTriggerOnce(false)
{
	bTickingTask = true;
	bSimulatedTask = true;
}

UWKAbilityTask_WaitMoveInput* UWKAbilityTask_WaitMoveInput::WaitMoveInput(UGameplayAbility* OwningAbility, bool OnlyTriggerOnce)
{
	UWKAbilityTask_WaitMoveInput* Task = NewAbilityTask<UWKAbilityTask_WaitMoveInput>(OwningAbility);
	Task->bOnlyTriggerOnce = OnlyTriggerOnce;
	return Task;
}

void UWKAbilityTask_WaitMoveInput::Activate()
{
	if (!IsLocallyControlled())
	{
		EndTask();
	}
	else
	{
		InputComponent = UWKPlayerCharacterInputComponent::FindPlayerCharacterInputComponent(GetAvatarActor());
		SetWaitingOnAvatar();
	}
}

void UWKAbilityTask_WaitMoveInput::TickTask(float DeltaTime)
{
	if (IsValid(InputComponent))
	{
		FVector2D MoveInput = InputComponent->MoveInput;
		FString Msg = FString::Printf(TEXT("MoveInput=(%f, %f)   %f"), MoveInput.X, MoveInput.Y, MoveInput.SquaredLength());
		GEngine->AddOnScreenDebugMessage(1, 0.2f, FColor::Green, Msg);
		if (MoveInput.SquaredLength() > 0.1f)
		{
			// 有移动输入
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnMoveInput.Broadcast(MoveInput);
			}

			if (bOnlyTriggerOnce)
			{
				EndTask();
			}
		}
	}
	else
	{
		EndTask();
	}
}

void UWKAbilityTask_WaitMoveInput::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
