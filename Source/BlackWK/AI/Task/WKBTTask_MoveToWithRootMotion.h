// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "WKBTTask_MoveToWithRootMotion.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKBTTask_MoveToWithRootMotion : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UWKBTTask_MoveToWithRootMotion(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CurrentTargetKey;

	UPROPERTY(EditAnywhere, Category="Config")
	float AcceptableRadius = 400.f;
};
