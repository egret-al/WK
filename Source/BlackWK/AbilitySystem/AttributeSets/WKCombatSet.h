// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAttributeSetBase.h"
#include "WKCombatSet.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKCombatSet : public UWKAttributeSetBase
{
	GENERATED_BODY()

public:
	UWKCombatSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ATTRIBUTE_ACCESSORS(UWKCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UWKCombatSet, BaseHeal);
	ATTRIBUTE_ACCESSORS(UWKCombatSet, Mana);
	ATTRIBUTE_ACCESSORS(UWKCombatSet, MaxMana);
	ATTRIBUTE_ACCESSORS(UWKCombatSet, ManaRecovery);
	ATTRIBUTE_ACCESSORS(UWKCombatSet, Resilience);
	ATTRIBUTE_ACCESSORS(UWKCombatSet, MaxResilience);
	ATTRIBUTE_ACCESSORS(UWKCombatSet, ResilienceRecovery);
	ATTRIBUTE_ACCESSORS(UWKCombatSet, Stamina);
	ATTRIBUTE_ACCESSORS(UWKCombatSet, MaxStamina);
	ATTRIBUTE_ACCESSORS(UWKCombatSet, StaminaRecovery);
	ATTRIBUTE_ACCESSORS(UWKCombatSet, ImpactAmount);

	mutable FWKAttributeEvent OnManaChanged;
	mutable FWKAttributeEvent OnMaxManaChanged;
	mutable FWKAttributeEvent OnStaminaChanged;
	mutable FWKAttributeEvent OnMaxStaminaChanged;
	mutable FWKAttributeEvent OnResilienceChanged;
	mutable FWKAttributeEvent OnMaxResilienceChanged;

protected:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

protected:
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_ImpactAmount(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ManaRecovery(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxResilience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ResilienceRecovery(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_StaminaRecovery(const FGameplayAttributeData& OldValue);

public:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "CombatSet|Combat")
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "CombatSet|Combat")
	FGameplayAttributeData BaseHeal;

	// 法力值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "CombatSet|Combat")
	FGameplayAttributeData Mana;

	// 最大法力值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "CombatSet|Combat")
	FGameplayAttributeData MaxMana;

	// 法力每秒恢复速率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRecovery, Category = "CombatSet|Combat")
	FGameplayAttributeData ManaRecovery;

	// 韧性值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "CombatSet|Combat")
	FGameplayAttributeData Resilience;

	// 最大韧性值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxResilience, Category = "CombatSet|Combat")
	FGameplayAttributeData MaxResilience;

	// 韧性值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ResilienceRecovery, Category = "CombatSet|Combat")
	FGameplayAttributeData ResilienceRecovery;

	// 体力值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "CombatSet|Combat")
	FGameplayAttributeData Stamina;

	// 最大体力值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "CombatSet|Combat")
	FGameplayAttributeData MaxStamina;

	// 体力每秒恢复速率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaRecovery, Category = "CombatSet|Combat")
	FGameplayAttributeData StaminaRecovery;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ImpactAmount, Category = "CombatSet|Combat")
	FGameplayAttributeData ImpactAmount;

private:
	float ManaBeforeAttributeChange;
	float MaxManaBeforeAttributeChange;
	float ResilienceBeforeAttributeChange;
	float MaxResilienceBeforeAttributeChange;
	float StaminaBeforeAttributeChange;
	float MaxStaminaBeforeAttributeChange;
};
