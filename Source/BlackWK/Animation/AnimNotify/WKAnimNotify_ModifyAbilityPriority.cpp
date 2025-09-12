// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotify_ModifyAbilityPriority.h"

#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/Abilities/WKGameplayAbility.h"
#include "BlackWK/Gameplay/WKGameplayStatics.h"

UWKAnimNotify_ModifyAbilityPriority::UWKAnimNotify_ModifyAbilityPriority()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor::Red;
#endif
}

void UWKAnimNotify_ModifyAbilityPriority::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!UWKGameplayStatics::WorldIsGame(MeshComp))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (Owner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// 模拟端不修改
		return;
	}

	UWKAbilitySystemComponent* ASC = UWKGameplayStatics::GetWKAbilitySystemComponent(Owner);
	if (!ASC)
	{
		return;
	}

	UWKGameplayAbility* CurrentPriorityAbility = ASC->GetCurrentPriorityAbility();
	if (!CurrentPriorityAbility)
	{
		return;
	}

	UWKGameplayAbilityDataAsset* AbilityDataAsset = CurrentPriorityAbility->AbilityDataAsset;
	if (!AbilityDataAsset || AbilityDataAsset->AbilityTag != SkillTag)
	{
		return;
	}

	ASC->UpdateCurrentPriorityAbility(CurrentPriorityAbility, AbilityPriorityInfo);
}

FString UWKAnimNotify_ModifyAbilityPriority::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Priority:%.2f"), AbilityPriorityInfo.Priority);
}
