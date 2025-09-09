// Fill out your copyright notice in the Description page of Project Settings.


#include "WKCombatSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UWKCombatSet::UWKCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
	, ImpactAmount(0.f)
{
}

void UWKCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, Mana, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, MaxMana, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, ManaRecovery, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, Resilience, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, MaxResilience, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, ResilienceRecovery, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, Stamina, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, MaxStamina, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, StaminaRecovery, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, StickEnergy, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, MaxStickEnergy, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWKCombatSet, ImpactAmount, COND_OwnerOnly, REPNOTIFY_Always);
}

bool UWKCombatSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	ManaBeforeAttributeChange = GetMana();
	MaxManaBeforeAttributeChange = GetMaxMana();
	StaminaBeforeAttributeChange = GetStamina();
	MaxStaminaBeforeAttributeChange = GetMaxStamina();
	ResilienceBeforeAttributeChange = GetResilience();
	MaxResilienceBeforeAttributeChange = GetMaxResilience();
	StickEnergyBeforeAttributeChange = GetStickEnergy();
	MaxStickEnergyBeforeAttributeChange = GetMaxStickEnergy();

	return true;
}

void UWKCombatSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetResilienceAttribute())
	{
		SetResilience(FMath::Clamp(GetResilience(), 0.f, GetMaxResilience()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetStickEnergyAttribute())
	{
		SetStickEnergy(FMath::Clamp(GetStickEnergy(), 0.f, GetMaxStickEnergy()));
	}


	if (GetMana() != ManaBeforeAttributeChange)
	{
		OnManaChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ManaBeforeAttributeChange, GetMana());
	}
	if (GetMaxMana() != MaxManaBeforeAttributeChange)
	{
		OnMaxManaChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxManaBeforeAttributeChange, GetMaxMana());
	}

	if (GetResilience() != ResilienceBeforeAttributeChange)
	{
		OnResilienceChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ResilienceBeforeAttributeChange, GetResilience());
	}
	if (GetMaxResilience() != MaxResilienceBeforeAttributeChange)
	{
		OnMaxResilienceChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxResilienceBeforeAttributeChange, GetMaxResilience());
	}

	if (GetStamina() != StaminaBeforeAttributeChange)
	{
		OnStaminaChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, StaminaBeforeAttributeChange, GetStamina());
	}
	if (GetMaxStamina() != MaxStaminaBeforeAttributeChange)
	{
		OnMaxStaminaChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxStaminaBeforeAttributeChange, GetMaxStamina());
	}

	if (GetStickEnergy() != StickEnergyBeforeAttributeChange)
	{
		OnStickEnergyChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, StickEnergyBeforeAttributeChange, GetStickEnergy());
	}
	if (GetMaxStickEnergy() != MaxStickEnergyBeforeAttributeChange)
	{
		OnMaxStickEnergyChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxStickEnergyBeforeAttributeChange, GetMaxStickEnergy());
	}
}

void UWKCombatSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UWKCombatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UWKCombatSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UWKCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, BaseDamage, OldValue);
}

void UWKCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, BaseHeal, OldValue);
}

void UWKCombatSet::OnRep_ImpactAmount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, ImpactAmount, OldValue);
}

void UWKCombatSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, Mana, OldValue);

	const float CurrentMana = GetMana();
	const float EstimatedMagnitude = CurrentMana - OldValue.GetCurrentValue();
	OnManaChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentMana);
}

void UWKCombatSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, MaxMana, OldValue);
	
	const float CurrentMaxMana = GetMaxMana();
	const float EstimatedMagnitude = CurrentMaxMana - OldValue.GetCurrentValue();
	OnMaxManaChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentMaxMana);
}

void UWKCombatSet::OnRep_ManaRecovery(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, ManaRecovery, OldValue);
}

void UWKCombatSet::OnRep_Resilience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, Resilience, OldValue);

	const float CurrentResilience = GetResilience();
	const float EstimatedMagnitude = CurrentResilience - OldValue.GetCurrentValue();
	OnResilienceChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentResilience);
}

void UWKCombatSet::OnRep_MaxResilience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, MaxResilience, OldValue);

	const float CurrentMaxResilience = GetMaxResilience();
	const float EstimatedMagnitude = CurrentMaxResilience - OldValue.GetCurrentValue();
	OnMaxResilienceChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentMaxResilience);
}

void UWKCombatSet::OnRep_ResilienceRecovery(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, ResilienceRecovery, OldValue);
}

void UWKCombatSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, Stamina, OldValue);

	const float CurrentStamina = GetStamina();
	const float EstimatedMagnitude = CurrentStamina - OldValue.GetCurrentValue();
	OnStaminaChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentStamina);
}

void UWKCombatSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, MaxStamina, OldValue);

	const float CurrentMaxStamina = GetMaxStamina();
	const float EstimatedMagnitude = CurrentMaxStamina - OldValue.GetCurrentValue();
	OnMaxStaminaChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentMaxStamina);
}

void UWKCombatSet::OnRep_StaminaRecovery(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, StaminaRecovery, OldValue);
}

void UWKCombatSet::OnRep_StickEnergy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, StickEnergy, OldValue);

	const float CurrentStickEnergy = GetStickEnergy();
	const float EstimatedMagnitude = CurrentStickEnergy - OldValue.GetCurrentValue();
	OnStickEnergyChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentStickEnergy);
}

void UWKCombatSet::OnRep_MaxStickEnergy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWKCombatSet, MaxStickEnergy, OldValue);

	const float CurrentMaxStickEnergy = GetMaxStickEnergy();
	const float EstimatedMagnitude = CurrentMaxStickEnergy - OldValue.GetCurrentValue();
	OnMaxStickEnergyChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentMaxStickEnergy);
}
