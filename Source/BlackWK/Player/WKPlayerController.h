// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WKPlayerController.generated.h"

class UWKDamageTextWidgetComponent;
class UWKWidgetHUD;
class AWKCharacterBase;

UCLASS()
class BLACKWK_API AWKPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void CreateHUD();
	UWKWidgetHUD* GetHUD();

	// 设置左右旋转限制角度(默认基于当前朝向)
	UFUNCTION(Blueprintcallable)
	void SetRotationYawLimitBaseNow(float LimitYawInDegrees = 0.f, FRotator InLimitRotationBase = FRotator::ZeroRotator);
	UFUNCTION(Blueprintcallable)
	void SetRotationPitchLimitBaseNow(float LimitPitchInDegrees = 0.f, FRotator InLimitRotationBase = FRotator::ZeroRotator);
	// 清除旋转限制
	UFUNCTION(Blueprintcallable)
	void ClearRotationLimit();

public:
	bool bHasRotationInputThisFrame;

protected:
	// Server only
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void UpdateRotation(float DeltaTime) override;

	/** 设置转向速度 */
	UFUNCTION(Blueprintcallable)
	void SetTurnRate(float NewRate);
	
	/** 重置转向速度 */
	UFUNCTION(Blueprintcallable)
	void ResetTurnRate();
	
protected:
	UPROPERTY(EditAnywhere, Category = "WKConfig|UI")
	TSubclassOf<UWKDamageTextWidgetComponent> DamageNumberClass;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WKConfig|UI")
	TSubclassOf<UWKWidgetHUD> UIHUDWidgetClass;
	
	UPROPERTY(BlueprintReadWrite, Category = "WKConfig|UI")
	UWKWidgetHUD* UIHUDWidget;



	// 是否锁定PC旋转
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLockControlRotation;

	// 转向速度比例
	float TurnRate = 1.f;

private:
	// 是否限制Yaw朝向
	bool bIsLimitRotationYaw = false;
	// 是否限制Pitch朝向
	bool bIsLimitRotationPitch = false;

	// 限制朝向时的基础朝向
	FRotator LimitRotationBase;

	//  限制朝向时可调整朝向
	float LimitYaw = 0.f;
	//  限制朝向时可调整朝向
	float LimitPitch = 0.f;
};
