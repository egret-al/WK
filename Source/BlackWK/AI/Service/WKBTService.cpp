// Fill out your copyright notice in the Description page of Project Settings.


#include "WKBTService.h"

#include "BlackWK/AI/WKAIController.h"

void UWKBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AWKAIController* OwnerController = Cast<AWKAIController>(OwnerComp.GetAIOwner());
	if (!OwnerController)
	{
		return;
	}
	OwnerBlackboardComponent = OwnerComp.GetBlackboardComponent();
}
