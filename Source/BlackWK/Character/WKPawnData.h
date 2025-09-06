// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WKPawnData.generated.h"

class UWKAnimationConfig;
class UWKInputConfig;
class UWKAbilitySet;

/**
 * 定义一个Pawn
 */
UCLASS(BlueprintType, Const, Meta = (ShortTooltip = "定义一个Pawn"))
class BLACKWK_API UWKPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UWKPawnData(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Pawn")
	TSubclassOf<APawn> PawnClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Abilities")
	TArray<TObjectPtr<UWKAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Input")
	TObjectPtr<UWKInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Animation")
	TObjectPtr<UWKAnimationConfig> AnimationConfig;
};
