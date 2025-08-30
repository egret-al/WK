// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimInstanceBase.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "WKDaShengBrokenAnimInstance.generated.h"

class UBlackboardComponent;
class AWKDaShengBrokenAIController;
class AWKAIDaShengCharacter;

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKDaShengBrokenAnimInstance : public UWKAnimInstanceBase
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void OnEnterStandIdle();

	UFUNCTION(BlueprintCallable)
	void OnEnterSitDownIdle();

protected:
	void UpdateState();
	void UpdateTreeInfo();
	void UpdateMovement();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "References")
	TWeakObjectPtr<AWKAIDaShengCharacter> OwnerDaSheng;

	UPROPERTY(BlueprintReadOnly, Category = "References")
	TObjectPtr<AWKDaShengBrokenAIController> OwnerController;

	UPROPERTY(BlueprintReadOnly, Category = "References")
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	bool bHasSawTarget = false;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	TWeakObjectPtr<AWKCharacterBase> CurrentTarget;

	UPROPERTY(BlueprintReadOnly)
	bool bHasStandIdle = false;

	UPROPERTY(BlueprintReadOnly, Category = "Move")
	float TurnAngle = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "BehaviorTree")
	bool bFollowTarget = false;

private:
	void InitInstanceInfo();
};
