// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WKCharacterBase.h"
#include "WKPlayerCharacterBase.generated.h"

class UWKFloatingStatusBarWidget;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BLACKWK_API AWKPlayerCharacterBase : public AWKCharacterBase
{
	GENERATED_BODY()

public:
	AWKPlayerCharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;

	USpringArmComponent* GetCameraBoom() const;
	UCameraComponent* GetFollowCamera() const;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	FVector GetStartingCameraBoomLocation();

	UWKFloatingStatusBarWidget* GetFloatingStatusBar();

	// USkeletalMeshComponent* GetGunComponent() const;

	virtual void FinishDying() override;

	FVector2D GetMoveInput() const;

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	void InitInput();

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Move_Complete(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_Lock(const FInputActionValue& InputActionValue);

	// Creates and initializes the floating status bar for heroes.
	// Safe to call many times because it checks to make sure it only executes once.
	UFUNCTION()
	void InitializeFloatingStatusBar();

	// Client only
	virtual void OnRep_PlayerState() override;

	// Called from both SetupPlayerInputComponent and OnRep_PlayerState because of a potential race condition where the PlayerController might
	// call ClientRestart which calls SetupPlayerInputComponent before the PlayerState is repped to the client so the PlayerState would be null in SetupPlayerInputComponent.
	// Conversely, the PlayerState might be repped before the PlayerController calls ClientRestart so the Actor's InputComponent would be null in OnRep_PlayerState.
	void BindAbilitySystemComponentInput();

protected:
	void UpdateLockTargetCameraLocation();
	
	void SetRotationMode(EWKRotationMode NewRotationMode);
	virtual void OnRotationModeChanged(EWKRotationMode NewRotationMode);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseTurnRate = 45.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	float StartingCameraBoomArmLength;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	FVector StartingCameraBoomLocation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	UCameraComponent* FollowCamera;

	// UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	// USkeletalMeshComponent* GunComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WKConfig|UI")
	TSubclassOf<UWKFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
	UWKFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI")
	UWidgetComponent* UIFloatingStatusBarComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Input")
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Input")
	TObjectPtr<UInputAction> IA_LookMouse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Input")
	TObjectPtr<UInputAction> IA_Lock;
	
	bool ASCInputBound = false;

	FGameplayTag DeadTag;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MoveInput;

protected:
	// 锁定目标
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AWKCharacterBase> LockTarget;

	// 旋转模式
	UPROPERTY(BlueprintReadOnly)
	EWKRotationMode RotationMode = EWKRotationMode::VelocityDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig|Camera")
	float InterpCameraLookingDirection = 10.f;
	
	// 记录的过去的位置信息
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> HistoryLocationQueue;

	// 记录的历史数量
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig|Camera")
	int32 HistorySize = 12;

	// 按下锁定时的控制器旋转
	UPROPERTY(BlueprintReadOnly)
	FRotator LockTargetSnapshotControlRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig|Lock")
	float LockInterpSpeed = 5.f;

	// 锁定旋转过渡时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WKConfig|Lock")
	float LockInterpDuration = 0.5f;

	// 插值曲线 (编辑器里可指定，比如 EaseInOut)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WKConfig|Lock")
	TObjectPtr<UCurveFloat> LockInterpCurve;

private:
	// 内部状态
	float LockInterpTime = 0.0f;
	bool bLockInterpInProgress = false;
	FRotator LockStartRotation;
};
