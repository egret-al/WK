// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKBTService.h"
#include "BehaviorTree/BTService.h"
#include "WKBTService_UpdateTargetLocation.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKBTService_UpdateTargetLocation : public UWKBTService
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// 当前跟随的目标
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WKConfig|Read")
	FBlackboardKeySelector CurrentTargetSelector;

	// 当前跟随的目标位置，这个是需要计算后设置回去的
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WKConfig|Write")
	FBlackboardKeySelector TargetLocationSelector;
};
