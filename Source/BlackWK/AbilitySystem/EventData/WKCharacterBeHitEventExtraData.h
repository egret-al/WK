// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WKCharacterBeHitEventExtraData.generated.h"

class AWKCharacterBase;
class UWKGameplayAbilityDataAsset;

/**
 * 
 */
UCLASS(BlueprintType)
class BLACKWK_API UWKCharacterBeHitEventExtraData : public UObject
{
	GENERATED_BODY()

public:
	// 攻击者
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AWKCharacterBase> Instigator;

	// 攻击的技能资产
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UWKGameplayAbilityDataAsset> AbilityDataAsset;

	// 造成的受击程度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BeHitImpact;

	// 造成的冲击力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BeHitImpulse;

	// 造成的冲击力方向
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector BeHitImpulseDirection;
};
