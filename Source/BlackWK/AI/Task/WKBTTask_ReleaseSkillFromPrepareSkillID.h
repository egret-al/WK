// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "WKBTTask_ReleaseSkillFromPrepareSkillID.generated.h"

struct FAbilityEndedData;
/**
 * 
 */
UCLASS()
class BLACKWK_API UWKBTTask_ReleaseSkillFromPrepareSkillID : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void OnAbilityEnded(const FAbilityEndedData& EndedData);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WKConfig|Read")
	FBlackboardKeySelector PrepareReleaseSkillIDSelector;
	
	UPROPERTY(Transient)
	TObjectPtr<UBehaviorTreeComponent> OwnerBehaviorTreeComponent;
	
	FDelegateHandle AbilityEndedHandle;
};
