// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "WKBTTask_ActivateAbilityByTag.generated.h"

/**
 * 根据Tag激活GA
 */
UCLASS()
class BLACKWK_API UWKBTTask_ActivateAbilityByTag : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "WKConfig")
	FGameplayTagContainer AbilityTagContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WKConfig")
	FBlackboardKeySelector CurrentTargetSelector;
};
