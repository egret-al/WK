// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimationTypes.h"
#include "Engine/DataAsset.h"
#include "WKAnimationConfig.generated.h"

/**
 * 角色的动画配置
 */
UCLASS()
class BLACKWK_API UWKAnimationConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	// 被命中的动画配置
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig|BeHit")
	FWKBeHitImpact BeHitImpact;
};
