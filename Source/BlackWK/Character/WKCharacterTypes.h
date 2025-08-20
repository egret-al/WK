// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WKCharacterTypes.generated.h"

USTRUCT(BlueprintType)
struct FWKEssentialValue
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector Acceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector MovementInput = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHasMovementInput = false;

	UPROPERTY(BlueprintReadOnly)
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float MovementInputAmount = 0.f;

	UPROPERTY(BlueprintReadOnly)
	FRotator AimingRotation = FRotator::ZeroRotator;
};

/**
 * 移动状态
 */
UENUM(BlueprintType)
enum class EWKMovementState : uint8
{
	None,
	Grounded,
	InAir
};

/**
 * 角色旋转模式
 */
UENUM(BlueprintType)
enum class EWKRotationMode : uint8
{
	None,
	VelocityDirection,
	LookingDirection
};

USTRUCT(BlueprintType)
struct FWKCharacterState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EWKMovementState MovementState = EWKMovementState::None;

	UPROPERTY(BlueprintReadOnly)
	EWKRotationMode RotationMode = EWKRotationMode::VelocityDirection;
};