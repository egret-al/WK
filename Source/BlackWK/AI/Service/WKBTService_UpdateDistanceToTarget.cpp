// Fill out your copyright notice in the Description page of Project Settings.


#include "WKBTService_UpdateDistanceToTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BlackWK/Character/WKCharacterBase.h"

void UWKBTService_UpdateDistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!OwnerBlackboardComponent)
	{
		return;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return;
	}
	
	AWKCharacterBase* Owner = Cast<AWKCharacterBase>(Controller->GetPawn());
	
	UObject* CurrentTarget = OwnerBlackboardComponent->GetValueAsObject(CurrentTargetSelector.SelectedKeyName);
	AActor* CurrentTargetActor = Cast<AActor>(CurrentTarget);
	if (!CurrentTargetActor)
	{
		return;
	}

	const float Dist2D = FVector::Dist2D(CurrentTargetActor->GetActorLocation(), Owner->GetActorLocation());
	OwnerBlackboardComponent->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, Dist2D);
}
