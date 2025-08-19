// Fill out your copyright notice in the Description page of Project Settings.


#include "WKDaShengBrokenAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BlackWK/Character/WKAIDaShengCharacter.h"
#include "Perception/AIPerceptionComponent.h"

AWKDaShengBrokenAIController::AWKDaShengBrokenAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

}

void AWKDaShengBrokenAIController::BeginPlay()
{
	Super::BeginPlay();

	AIPerception->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnTargetPerceptionUpdated);

	DaShengPawn = Cast<AWKAIDaShengCharacter>(GetPawn());
	RunBehaviorTree(BehaviorTree);
}

void AWKDaShengBrokenAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AIPerception->OnTargetPerceptionUpdated.RemoveDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
	Super::EndPlay(EndPlayReason);
}

bool AWKDaShengBrokenAIController::HasSawEnemyTarget()
{
	return bHasSawTarget;
}

void AWKDaShengBrokenAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	AWKCharacterBase* InTarget = Cast<AWKCharacterBase>(Actor);
	if (!InTarget)
	{
		return;
	}

	// 已经有目标了
	if (bHasSawTarget)
	{
		return;
	}

	// 是否成功看见了
	if (Stimulus.WasSuccessfullySensed())
	{
		bHasSawTarget = true;
	}

	// 更新行为树
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	BlackboardComponent->SetValueAsBool(FName(TEXT("IsSeeingTarget")), Stimulus.WasSuccessfullySensed());
	BlackboardComponent->SetValueAsObject(FName(TEXT("CurrentTarget")), Actor);
}
