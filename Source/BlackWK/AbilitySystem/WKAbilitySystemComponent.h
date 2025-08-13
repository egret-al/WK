// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WKAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UWKAbilitySystemComponent*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLACKWK_API UWKAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void ReceiveDamage(UWKAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);
	
public:
	bool bCharacterAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;
	FReceivedDamageDelegate ReceivedDamage;
};
