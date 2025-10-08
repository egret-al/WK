// Fill out your copyright notice in the Description page of Project Settings.


#include "WKBTTask_ReleaseSkillFromPrepareSkillID.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/Abilities/WKGameplayAbility.h"
#include "BlackWK/AbilitySystem/DataAssets/WKDataTypes.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/Data/WKGameplaySettings.h"

EBTNodeResult::Type UWKBTTask_ReleaseSkillFromPrepareSkillID::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent = Controller->GetBlackboardComponent();
	
	int32 SkillID = BlackboardComponent->GetValueAsInt(PrepareReleaseSkillIDSelector.SelectedKeyName);
	if (SkillID <= 0)
	{
		return EBTNodeResult::Failed;
	}

	UWKGameplaySettings* GameplaySettings = UWKGameplaySettings::GetGameplaySettings();
	if (!GameplaySettings)
	{
		return EBTNodeResult::Failed;
	}

	AWKCharacterBase* Owner = Cast<AWKCharacterBase>(Controller->GetPawn());
	if (!Owner)
	{
		return EBTNodeResult::Failed;
	}

	UWKAbilitySystemComponent* ASC = Owner->GetWKAbilitySystemComponent();
	if (!Owner)
	{
		return EBTNodeResult::Failed;
	}

	// 查表
	FWKSkillTableRow SkillTableRow;
	if (!ASC->GetSkillRowByID(SkillID, SkillTableRow))
	{
		return EBTNodeResult::Failed;
	}
	FGameplayTag ActivateTag = SkillTableRow.SkillTag;

	// 没有找到Tag
	if (!ActivateTag.IsValid())
	{
		return EBTNodeResult::Failed;
	}
	
	if (ASC->TryActivateAbilitiesByTag(ActivateTag.GetSingleTagContainer()))
	{
		OwnerBehaviorTreeComponent = OwnerComp;
		// 激活成功，监听GA结束
		AbilityEndedHandle = ASC->OnAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnded);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

void UWKBTTask_ReleaseSkillFromPrepareSkillID::OnAbilityEnded(const FAbilityEndedData& EndedData)
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

	if (OwnerBehaviorTreeComponent)
	{
		if (AAIController* Controller = OwnerBehaviorTreeComponent->GetAIOwner())
		{
			if (UBlackboardComponent* BlackboardComponent = Controller->GetBlackboardComponent())
			{
				BlackboardComponent->SetValueAsInt(PrepareReleaseSkillIDSelector.SelectedKeyName, INDEX_NONE);
			}
		}
	}
	FinishLatentTask(*OwnerBehaviorTreeComponent, EBTNodeResult::Succeeded);
}
