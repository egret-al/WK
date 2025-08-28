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
	ATTRIBUTE_ACCESSORS(UWKCombatSet, ImpactAmount);

protected:
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_ImpactAmount(const FGameplayAttributeData& OldValue);

public:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "CombatSet|Combat")
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "CombatSet|Combat")
	FGameplayAttributeData BaseHeal;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ImpactAmount, Category = "CombatSet|Combat")
	FGameplayAttributeData ImpactAmount;
};
