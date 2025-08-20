// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlackWK/Character/WKCharacterTypes.h"
#include "WKAnimInstanceBase.generated.h"

class AWKCharacterBase;
/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void UpdateCharacterInfo();

protected:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AWKCharacterBase> OwnerWK;

	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly)
	FVector Acceleration;

	UPROPERTY(BlueprintReadOnly)
	FVector MovementInput;

	UPROPERTY(BlueprintReadOnly)
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHasMovementInput = false;

	UPROPERTY(BlueprintReadOnly)
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float MovementInputAmount = 0.f;

	UPROPERTY(BlueprintReadOnly)
	FRotator AimingRotation;

	UPROPERTY(BlueprintReadOnly)
	EWKMovementState MovementState = EWKMovementState::None;

	UPROPERTY(BlueprintReadOnly)
	EWKRotationMode RotationMode = EWKRotationMode::None;
};
