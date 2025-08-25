// Fill out your copyright notice in the Description page of Project Settings.


#include "WKCombatSet.h"

#include "Net/UnrealNetwork.h"

UWKCombatSet::UWKCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void UWKCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void UWKCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, BaseDamage, OldValue);
}

void UWKCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, BaseHeal, OldValue);
}
