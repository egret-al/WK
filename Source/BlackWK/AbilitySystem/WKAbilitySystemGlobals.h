// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "WKAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class BLACKWK_API UWKAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	UWKAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//~UAbilitySystemGlobals interface
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	//~End of UAbilitySystemGlobals interface
};
