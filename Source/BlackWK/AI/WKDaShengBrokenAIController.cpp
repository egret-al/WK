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

AWKCharacterBase* AWKDaShengBrokenAIController::GetCurrentTarget()
{
	return CurrentTarget.Get();
}

void AWKDaShengBrokenAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 已经有目标了
	if (Stimulus.WasSuccessfullySensed() && bHasSawTarget)
	{
		return;
	}

	if (!Stimulus.WasSuccessfullySensed())
	{
		// 丢失目标
		bHasSawTarget = false;
		CurrentTarget = nullptr;
	}
	else
	{
		bHasSawTarget = true;
		CurrentTarget = Cast<AWKCharacterBase>(Actor);
	}
	// 更新行为树
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	BlackboardComponent->SetValueAsBool(FName(TEXT("HasSeeingTarget")), bHasSawTarget);
	BlackboardComponent->SetValueAsObject(FName(TEXT("CurrentTarget")), CurrentTarget.Get());
}
