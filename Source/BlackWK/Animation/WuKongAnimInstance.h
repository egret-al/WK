// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimInstanceBase.h"
#include "WKAnimInstanceExtensionInterface.h"
#include "WuKongAnimInstance.generated.h"

class AWuKongPlayerCharacter;

UCLASS()
class BLACKWK_API UWuKongAnimInstance : public UWKAnimInstanceBase, public IWKAnimInstanceExtensionInterface
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativePostEvaluateAnimation() override;

	void UpdateInput();
	void UpdateRotation();

	// 向左进入大旋转
	UFUNCTION(BlueprintCallable)
	void OnEnterTurnLeft180();

	// 向右进入大旋转
	UFUNCTION(BlueprintCallable)
	void OnEnterTurnRight180();

	UFUNCTION(BlueprintCallable)
	void ResetTurn180();

	void UpdateTurn180();

	// 向左起步
	UFUNCTION(BlueprintCallable)
	void OnRunStartLeft();

	// 向右起步
	UFUNCTION(BlueprintCallable)
	void OnRunStartRight();

	// 重置起步标识
	UFUNCTION(BlueprintCallable)
	void ResetRunStart();

	void CalcTurnAngleFinal(
		float InTurnAngle,
		float InTurnAngle1,
		float InTurnAngle2,
		float InTurnAngleL1,
		float InTurnAngleL2,
		float InTurnAngleR1,
		float InTurnAngleR2,
		int32 InRotationNum,
		bool bIsCircleL,
		bool bIsCircleR,
		float& OutTurnAngleL,
		float& OutTurnAngleR,
		float& OutTurnAngleFinal,
		int32& OutRotationNum,
		bool& bOutCircleL,
		bool& bOutCircleR);

public:
	virtual void ModifyRootMotionTransform(FTransform& InoutTransform) override;
	
protected:
	// 是否忽略RootMotion的旋转
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RootMotion")
	uint8 bIgnoreMontageRootMotionRotation : 1;

	UPROPERTY(BlueprintReadOnly, Category = "RootMotion")
	FQuat RootRotationCache = FQuat::Identity;
	
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	FVector2D MoveInput;
	
	/// Ref
	UPROPERTY(BlueprintReadOnly, Category = "References")
	TWeakObjectPtr<AWuKongPlayerCharacter> OwnerWuKong;

	// 平面速度
	UPROPERTY(BlueprintReadOnly, Category = "Move")
	FVector2D VelocityPlane;

	UPROPERTY(BlueprintReadOnly, Category = "Move")
	float VelocityPlaneLength;

	// 旋转角度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	float TurnAngle = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	float TurnAngle180 = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	bool bEnterTurnLeft180 = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	bool bEnterTurnRight180 = false;


	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float TurnAngle1;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float TurnAngle2;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float TurnAngleAvg;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float TurnAngleL;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float TurnAngleL1;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float TurnAngleL2;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float TurnAngleR;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float TurnAngleR1;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float TurnAngleR2;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float TurnAngleFinal;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	int32 RotationNum = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float AccelerationLength;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float LastAccelerationLength;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	int32 ContinuousEmptyAccelNum = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	int32 MaxContinuousEmptyAccelNum = 5;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	bool bStop = false;
	
	// 起步
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	bool bRunStartL = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	bool bRunStartR = false;

private:
	FTransform TurnTransform180;

	bool bCanTurn;
	int32 CanTurnFrameWindow = 0;
};
