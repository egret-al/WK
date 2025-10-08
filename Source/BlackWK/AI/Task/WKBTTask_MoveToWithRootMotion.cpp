// Fill out your copyright notice in the Description page of Project Settings.


#include "WKBTTask_MoveToWithRootMotion.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UWKBTTask_MoveToWithRootMotion::UWKBTTask_MoveToWithRootMotion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = TEXT("MoveToWithRootMotion");
	bNotifyTick = true;
}

EBTNodeResult::Type UWKBTTask_MoveToWithRootMotion::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UWKBTTask_MoveToWithRootMotion::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;

	if (!BlackboardComponent || !Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(CurrentTargetKey.SelectedKeyName));
	if (!Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	const FVector TargetLocation = Target->GetActorLocation();
	const float DistSq = FVector::DistSquared2D(Pawn->GetActorLocation(), TargetLocation);

	if (DistSq <= AcceptableRadius * AcceptableRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		int32 SkillID = BlackboardComponent->GetValueAsInt(PrepareReleaseSkillIDSelector.SelectedKeyName);
		if (SkillID > 0)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
}
