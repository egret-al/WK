// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilityTask_PlayMontageAndWaitForEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UWKAbilityTask_PlayMontageAndWaitForEvent::UWKAbilityTask_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MontageToPlay(nullptr)
	, AnimRootMotionTranslationScale(1)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
	StartTime = 0.f;
	bBroadcastMontageStop = false;
	bMontageStop = false;
}

UWKAbilityTask_PlayMontageAndWaitForEvent* UWKAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	UAnimMontage* MontageToPlay,
	FGameplayTagContainer EventTags,
	float Rate,
	FName StartSection,
	float StartTime,
	bool bStopWhenAbilityEnds,
	float AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UWKAbilityTask_PlayMontageAndWaitForEvent* AbilityTask = NewAbilityTask<UWKAbilityTask_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);
	AbilityTask->MontageToPlay = MontageToPlay;
	AbilityTask->EventTags = EventTags;
	AbilityTask->Rate = Rate;
	AbilityTask->StartSection = StartSection;
	AbilityTask->StartTime = StartTime;
	AbilityTask->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	AbilityTask->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return AbilityTask;
}

void UWKAbilityTask_PlayMontageAndWaitForEvent::Activate()
{
	if (Ability == nullptr)
	{
		EndTask();
		return;
	}
	
	if (!IsValid(MontageToPlay))
	{
		ABILITY_LOG(Warning, TEXT("UDSAbilityTask_PlayMontageAndWaitForEvent Montage is null"));
		
		EndTask();
		return;
	}

	bool bPlayedMontage = false;

	if (AbilitySystemComponent.IsValid())
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			// Bind to event callback
			if (!EventTags.IsEmpty())
			{
				EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UWKAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent));
			}

			//AbilitySystemComponent->SetMontageRepAnimPositionMethod(ERepAnimPositionMethod::CurrentSectionId);
			if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection,StartTime) > 0.f)
			{
				// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UWKAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &UWKAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UWKAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
								  (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else
		{
			ABILITY_LOG(Warning, TEXT("UBDAbilityTask_PlayMontageAndWaitForEvent call to PlayMontage failed!"));
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UBDAbilityTask_PlayMontageAndWaitForEvent called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UBDAbilityTask_PlayMontageAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay),*InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
			BroadcastMontageStop();
		}
	}

	SetWaitingOnAvatar();
}

void UWKAbilityTask_PlayMontageAndWaitForEvent::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
}

void UWKAbilityTask_PlayMontageAndWaitForEvent::ExternalCancel()
{
	check(AbilitySystemComponent.IsValid());

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UWKAbilityTask_PlayMontageAndWaitForEvent::OnDestroy(bool bInOwnerFinished)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (bInOwnerFinished && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}
	
	BroadcastMontageStop();
	
	Super::OnDestroy(bInOwnerFinished);
}

FString UWKAbilityTask_PlayMontageAndWaitForEvent::GetDebugString() const
{
	TObjectPtr<UAnimMontage> PlayingMontage = nullptr;
	if (Ability)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay.Get() : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}

void UWKAbilityTask_PlayMontageAndWaitForEvent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UWKAbilityTask_PlayMontageAndWaitForEvent, MontageToPlay, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UWKAbilityTask_PlayMontageAndWaitForEvent, Rate, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UWKAbilityTask_PlayMontageAndWaitForEvent, StartTime, COND_InitialOnly);

}

bool UWKAbilityTask_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
	if (bMontageStop)
	{
		return true;
	}
	bMontageStop = true;
	
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return false;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if (AbilitySystemComponent.IsValid() && Ability)
	{
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability
			&& AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}
			
			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}
	return false;
}

void UWKAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	const bool bPlayingThisMontage = Montage == MontageToPlay && Ability && Ability->GetCurrentMontage() == MontageToPlay;
	if (bPlayingThisMontage)
	{
		// Reset AnimRootMotionTranslationScale
		ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
		if (Character &&
			(Character->GetLocalRole() == ROLE_Authority
				|| (Character->GetLocalRole() == ROLE_AutonomousProxy
				&& Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)
			))
		{
			Character->SetAnimRootMotionTranslationScale(1.f);
		}
		
		if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
		{
			ASC->ClearAnimatingAbility(Ability);
		}
	}
	
	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
			BroadcastMontageStop();
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
			BroadcastMontageStop();
		}
	}
}

void UWKAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
	// TODO: Merge this fix back to engine, it was calling the wrong callback

	if (bStopWhenAbilityEnds)
	{
		StopPlayingMontage();
	}
	// Let the BP handle the interrupt as well
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UWKAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
			BroadcastMontageStop();
		}
	}

	EndTask();
}

void UWKAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

void UWKAbilityTask_PlayMontageAndWaitForEvent::BroadcastMontageStop()
{
	if (!bBroadcastMontageStop)
	{
		bBroadcastMontageStop = true;
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnMontageStop.Broadcast(FGameplayTag(), FGameplayEventData());
		}

		if (AbilitySystemComponent.IsValid())
		{
			AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
		}
	}
}
