// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "WKImpactExecution.generated.h"

/**
 * 冲击力造成的计算
 */
UCLASS()
class BLACKWK_API UWKImpactExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UWKImpactExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
