// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKBTService.h"
#include "WKBTService_UpdatePrepareReleaseSkillID.generated.h"

struct FWKSkillTableRow;
/**
 * 
 */
UCLASS()
class BLACKWK_API UWKBTService_UpdatePrepareReleaseSkillID : public UWKBTService
{
	GENERATED_BODY()

public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WKConfig|Write")
	FBlackboardKeySelector PrepareReleaseSkillIDSelector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WKConfig|Read")
	FBlackboardKeySelector DistanceToTargetSelector;
};
