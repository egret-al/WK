// Fill out your copyright notice in the Description page of Project Settings.


#include "WKPlayerState.h"

#include "BlackWK/AbilitySystem/WKAbilitySet.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/AttributeSets/WKHealthSet.h"
#include "BlackWK/AbilitySystem/AttributeSets/WKCombatSet.h"
#include "BlackWK/Character/WKPawnData.h"
#include "Net/UnrealNetwork.h"

AWKPlayerState::AWKPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UWKAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	NetUpdateFrequency = 100.0f;

	HealthSet = CreateDefaultSubobject<UWKHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UWKCombatSet>(TEXT("CombatSet"));
}

void AWKPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
}

void AWKPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		//TODO
		SetPawnData(PawnData);
	}
}

UAbilitySystemComponent* AWKPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UWKAbilitySystemComponent* AWKPlayerState::GetWKAbilitySystemComponent() const
{
	return Cast<UWKAbilitySystemComponent>(GetAbilitySystemComponent());
}

void AWKPlayerState::SetPawnData(const UWKPawnData* InPawnData)
{
	if (!InPawnData)
	{
		return;
	}

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	if (GetNetMode() == NM_Standalone)
	{
		OnRep_PawnData();
	}

	for (const TObjectPtr<UWKAbilitySet> AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet.Get())
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
	ForceNetUpdate();
}

bool AWKPlayerState::IsAlive() const
{
	return HealthSet->GetHealth() > 0.f;
}

float AWKPlayerState::GetHealth() const
{
	return HealthSet->GetHealth();
}

float AWKPlayerState::GetMaxHealth() const
{
	return HealthSet->GetMaxHealth();
}

void AWKPlayerState::OnRep_PawnData()
{
}
