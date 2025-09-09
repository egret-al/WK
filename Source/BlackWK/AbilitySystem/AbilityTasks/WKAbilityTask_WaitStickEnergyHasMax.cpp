// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilityTask_WaitStickEnergyHasMax.h"

#include "BlackWK/AbilitySystem/AttributeSets/WKCombatSet.h"
#include "BlackWK/Character/Components/WKHealthComponent.h"

UWKAbilityTask_WaitStickEnergyHasMax::UWKAbilityTask_WaitStickEnergyHasMax(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UWKAbilityTask_WaitStickEnergyHasMax* UWKAbilityTask_WaitStickEnergyHasMax::WaitStickEnergyHasMax(UGameplayAbility* OwningAbility)
{
	UWKAbilityTask_WaitStickEnergyHasMax* NewTask = NewAbilityTask<UWKAbilityTask_WaitStickEnergyHasMax>(OwningAbility);
	return NewTask;
}

void UWKAbilityTask_WaitStickEnergyHasMax::Activate()
{
	UWKHealthComponent* HealthComponent = UWKHealthComponent::FindHealthComponent(GetAvatarActor());
	if (!HealthComponent)
	{
		EndTask();
		return;
	}

	HealthComponent->OnStickEnergyChanged.AddUniqueDynamic(this, &ThisClass::OnStickEnergyChanged);
}

void UWKAbilityTask_WaitStickEnergyHasMax::OnDestroy(bool bInOwnerFinished)
{
	if (UWKHealthComponent* HealthComponent = UWKHealthComponent::FindHealthComponent(GetAvatarActor()))
	{
		HealthComponent->OnStickEnergyChanged.RemoveDynamic(this, &ThisClass::OnStickEnergyChanged);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UWKAbilityTask_WaitStickEnergyHasMax::OnStickEnergyChanged(UWKHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator)
{
	float MaxStickEnergy = HealthComponent->GetMaxStickEnergy();
	if (FMath::IsNearlyEqual(MaxStickEnergy, NewValue))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnMaxDelegate.Broadcast();
		}
	}
}

