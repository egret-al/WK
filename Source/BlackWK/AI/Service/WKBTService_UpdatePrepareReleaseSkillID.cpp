// Fill out your copyright notice in the Description page of Project Settings.


#include "WKBTService_UpdatePrepareReleaseSkillID.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/DataAssets/WKDataTypes.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/Character/Components/WKSkillComponent.h"
#include "BlackWK/Data/WKGameplaySettings.h"

void UWKBTService_UpdatePrepareReleaseSkillID::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(Owner->GetAbilitySystemComponent());
	if (!ASC)
	{
		return;
	}

	// 当前与目标的距离
	const float DistToTarget = OwnerBlackboardComponent->GetValueAsFloat(DistanceToTargetSelector.SelectedKeyName);
	const int32 PrepareSkillID = OwnerBlackboardComponent->GetValueAsInt(PrepareReleaseSkillIDSelector.SelectedKeyName);
	if (PrepareSkillID > 0)
	{
		// 已经有技能选择好了
		return;
	}

	// 根据这个距离，查找在距离范围的所有技能ID
	UWKGameplaySettings* GameplaySettings = UWKGameplaySettings::GetGameplaySettings();
	if (!GameplaySettings)
	{
		return;
	}

	TArray<FWKSkillTableRow> TableRowsInRange;
	TArray<FWKSkillTableRow> SkillTableRows = ASC->GetSkillTableRows();
	
	for (const FWKSkillTableRow& TableRow : SkillTableRows)
	{
		// 检查范围
		if (DistToTarget >= TableRow.ReleaseDistCondition.X && DistToTarget <= TableRow.ReleaseDistCondition.Y)
		{
			// 检查CD
			if (UWKSkillComponent* SkillComponent = Owner->GetSkillComponent())
			{
				if (SkillComponent->IsSkillInCooldown(TableRow.SkillID))
				{
					continue;
				}
				TableRowsInRange.Add(TableRow);	
			}
		}
	}

	int32 SkillID = INDEX_NONE;
	if (!TableRowsInRange.IsEmpty())
	{
		// 随机选一个技能放
		const FWKSkillTableRow& ReleaseSkill = TableRowsInRange[FMath::RandRange(0, TableRowsInRange.Num() - 1)];
		SkillID = ReleaseSkill.SkillID;
	}
	OwnerBlackboardComponent->SetValueAsInt(PrepareReleaseSkillIDSelector.SelectedKeyName, SkillID);
}
