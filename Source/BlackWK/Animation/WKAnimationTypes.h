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
