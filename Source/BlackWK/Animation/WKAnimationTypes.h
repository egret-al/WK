// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WKAnimationTypes.generated.h"

USTRUCT(BlueprintType)
struct FWKVelocityBlend
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float F = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float B = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float L = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float R = 0.f;
};

UENUM(BlueprintType)
enum class EWKMovementDirection : uint8
{
	Forward,
	Right,
	Left,
	Backward
};

// 动画方向
UENUM(BlueprintType)
enum class EWKAnimDirection : uint8
{
	F,
	R,
	L,
	B
};

USTRUCT(BlueprintType)
struct FWKDirectionHitImpact
{
	GENERATED_BODY()

	// 击中强度，决定受击动画的受击程度
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HitImpact = 0.f;

	// 该击中强度下，可选的动画蒙太奇的SectionName
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> SectionNames;
};

USTRUCT(BlueprintType)
struct FWKDirectionHitImpactAmount
{
	GENERATED_BODY()

	// 包含各个受击程度下的受击SectionName数组
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWKDirectionHitImpact> BeHitImpacts;
};

USTRUCT(BlueprintType)
struct FWKBeHitImpact
{
	GENERATED_BODY()

	// 受击动画蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> BeHitMontage;
	
	// 各个方向的受击配置
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	TMap<EWKAnimDirection, FWKDirectionHitImpactAmount> BeHitImpactDirections;
};