// Fill out your copyright notice in the Description page of Project Settings.


#include "WKBTService_UpdateTargetLocation.h"

#include "BehaviorTree/BlackboardComponent.h"

void UWKBTService_UpdateTargetLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!OwnerBlackboardComponent)
	{
		return;
	}

	UObject* CurrentTarget = OwnerBlackboardComponent->GetValueAsObject(CurrentTargetSelector.SelectedKeyName);
	AActor* CurrentTargetActor = Cast<AActor>(CurrentTarget);
	if (!CurrentTargetActor)
	{
		return;
	}

	FVector ActorLocation = CurrentTargetActor->GetActorLocation();
	OwnerBlackboardComponent->SetValueAsVector(TargetLocationSelector.SelectedKeyName, ActorLocation);
}
