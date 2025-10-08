// Fill out your copyright notice in the Description page of Project Settings.


#include "WKBTService_UpdateIsPlayingMontage.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BlackWK/Character/WKCharacterBase.h"

void UWKBTService_UpdateIsPlayingMontage::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
	if (!Owner)
	{
		return;
	}

	UAnimMontage* CurrentMontage = Owner->GetCurrentMontage();
	bool bIsPlaying = CurrentMontage != nullptr;
	
	OwnerBlackboardComponent->SetValueAsBool(IsPlayingMontageSelector.SelectedKeyName, bIsPlaying);
}
