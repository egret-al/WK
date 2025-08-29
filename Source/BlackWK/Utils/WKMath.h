// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BLACKWK_API FWKMath
{
public:
	/**
	* Returns degree of the angle between InVector and InForward vector
	* The range of result angle is [-180, 180].
	*/
	static float CalculateDirection(const FVector& InVector, const FVector& InForward, const FVector& InRight);
	static float CalculateDirection(const FVector& InVector, const FVector& InForward);
	static float CalculateDirection(const FVector& InVector, const FRotator& BaseRotation);
};
