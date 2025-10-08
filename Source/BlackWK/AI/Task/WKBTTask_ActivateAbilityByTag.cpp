// Fill out your copyright notice in the Description page of Project Settings.


#include "WKBTTask_ActivateAbilityByTag.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/Character/WKCharacterBase.h"

EBTNodeResult::Type UWKBTTask_ActivateAbilityByTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent = Controller->GetBlackboardComponent();
	
	UObject* CurrentTarget = BlackboardComponent->GetValueAsObject(CurrentTargetSelector.SelectedKeyName);
	AActor* CurrentTargetActor = Cast<AActor>(CurrentTarget);
	if (!CurrentTargetActor)
	{
		return EBTNodeResult::Failed;
	}

	AWKCharacterBase* Owner = Cast<AWKCharacterBase>(Controller->GetPawn());
	if (!Owner)
	{
		return EBTNodeResult::Failed;
	}

	float Dist2D = FVector::Dist2D(CurrentTargetActor->GetActorLocation(), Owner->GetActorLocation());

	
	UWKAbilitySystemComponent* ASC = Owner->GetWKAbilitySystemComponent();
	if (!Owner)
	{
		return EBTNodeResult::Failed;
	}

	TArray<FGameplayTag> ActivateTags;
	AbilityTagContainer.GetGameplayTagArray(ActivateTags);
	FGameplayTag ActivateTag = ActivateTags[FMath::RandRange(0, ActivateTags.Num() - 1)];
	if (ASC->TryActivateAbilitiesByTag(ActivateTag.GetSingleTagContainer()))
	{
		OwnerBehaviorTreeComponent = OwnerComp;
		// 激活成功，监听GA结束
		AbilityEndedHandle = ASC->OnAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnded);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

void UWKBTTask_ActivateAbilityByTag::OnAbilityEnded(const FAbilityEndedData& EndedData)
{
	if (!EndedData.AbilityThatEnded)
	{
		return;
	}
	
	UAbilitySystemComponent* ASC = EndedData.AbilityThatEnded->GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	ASC->OnAbilityEnded.Remove(AbilityEndedHandle);
	FinishLatentTask(*OwnerBehaviorTreeComponent, EBTNodeResult::Succeeded);
}
