// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "WKBTService.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKBTService : public UBTService
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UBlackboardComponent> OwnerBlackboardComponent;
};
