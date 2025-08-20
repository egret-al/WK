// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimationTypes.h"
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

	void UpdateAimingValues();
	void UpdateMovementValues();
	bool ShouldMoveCheck() const;
	void UpdateRotationValues();

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

public:
	virtual void ModifyRootMotionTransform(FTransform& InoutTransform) override;
	
protected:
	// 是否忽略RootMotion的旋转
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RootMotion")
	uint8 bIgnoreMontageRootMotionRotation : 1;

	UPROPERTY(BlueprintReadOnly, Category = "RootMotion")
	FQuat RootRotationCache = FQuat::Identity;
	
	/// Ref
	UPROPERTY(BlueprintReadOnly, Category = "References")
	TWeakObjectPtr<AWuKongPlayerCharacter> OwnerWuKong;

	// 旋转角度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	float TurnAngle = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	float TurnAngle180 = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	bool bEnterTurnLeft180 = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	bool bEnterTurnRight180 = false;
	
	// 起步
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	bool bRunStartL = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rotation")
	bool bRunStartR = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	bool bShouldMove = false;

	UPROPERTY(BlueprintReadOnly, Category = "Move")
	FWKVelocityBlend VelocityBlend;

	UPROPERTY(BlueprintReadOnly, Category = "Move")
	float TurnBlend = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Move")
	float RunPlayRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WKConfig")
	float VelocityBlendInterpSpeed = 12.f;

private:
	float CalculateTurnBlend() const;
	
	/** 计算速度混合度 */
	FWKVelocityBlend CalculateVelocityBlend() const;
	
	/** 插值速度混合度 */
	static FWKVelocityBlend InterpVelocityBlend(const FWKVelocityBlend& Current, const FWKVelocityBlend& Target, float InterpSpeed, float DeltaTime);
	
private:
	FTransform TurnTransform180;

	bool bCanTurn;
	int32 CanTurnFrameWindow = 0;
};
