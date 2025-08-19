// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAIController.h"
#include "WKAIStateInterface.h"
#include "WKDaShengBrokenAIController.generated.h"

class AWKCharacterBase;
struct FAIStimulus;
class AWKAIDaShengCharacter;

/**
 * 大圣残躯AI控制器
 */
UCLASS()
class BLACKWK_API AWKDaShengBrokenAIController : public AWKAIController, public IWKAIStateInterface
{
	GENERATED_BODY()

public:
	AWKDaShengBrokenAIController();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/// IWKAIStateInterface
	virtual bool HasSawEnemyTarget() override;
	/// ~IWKAIStateInterface

protected:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> AIPerception;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AWKAIDaShengCharacter> DaShengPawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(BlueprintReadOnly)
	bool bHasSawTarget = false;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AWKCharacterBase> CurrentTarget;
};
