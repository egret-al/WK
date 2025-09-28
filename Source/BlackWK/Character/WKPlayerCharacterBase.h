// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WKCharacterBase.h"
#include "WKPlayerCharacterBase.generated.h"

class USphereComponent;
class AWKPlayerController;
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
	virtual void OnRep_PlayerState() override;

	virtual void InitializeAbilityClient();

	USpringArmComponent* GetCameraBoom() const;
	UCameraComponent* GetFollowCamera() const;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	FVector GetStartingCameraBoomLocation();

	virtual bool IsPlayerCharacter() override;
	virtual AWKCharacterBase* GetCurrentAttackLockTarget() const override;
	
	FVector2d GetMoveInput() const;

	UFUNCTION(BlueprintPure)
	AWKPlayerController* GetPlayerController() const;

	// 获取最近的攻击追踪范围内的一个目标
	UFUNCTION(BlueprintPure)
	AWKCharacterBase* GetNearestAttackTrackingTarget() const;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;

	void Input_Lock(const FInputActionValue& InputActionValue);

protected:
	void UpdateLockTargetCameraLocation();
	
	void SetRotationMode(EWKRotationMode NewRotationMode);
	virtual void OnRotationModeChanged(EWKRotationMode NewRotationMode);

	UFUNCTION()
	void OnAttackTrackingBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackTrackingEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
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
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// 攻击追踪
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Combat")
	TObjectPtr<USphereComponent> AttackTracking; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Input")
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Input")
	TObjectPtr<UInputAction> IA_Lock;
	
	bool ASCInputBound = false;

protected:
	// 锁定目标
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AWKCharacterBase> LockTarget;

	// 旋转模式
	UPROPERTY(BlueprintReadOnly)
	EWKRotationMode RotationMode = EWKRotationMode::VelocityDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig|Camera")
	float InterpCameraLookingDirection = 10.f;

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

	// 在攻击追踪范围内的敌人
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WKConfig|Combat")
	TArray<AWKCharacterBase*> AttackTrackingTargets;

private:
	// 内部状态
	float LockInterpTime = 0.0f;
	bool bLockInterpInProgress = false;
	FRotator LockStartRotation;
};
