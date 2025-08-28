// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "WKDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UWKDamageExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
