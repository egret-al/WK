// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotifyState_AddGameplayTag.h"

#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/Gameplay/WKGameplayStatics.h"

void UWKAnimNotifyState_AddGameplayTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (UWKAbilitySystemComponent* ASC = UWKGameplayStatics::GetWKAbilitySystemComponent(MeshComp->GetOwner()))
	{
		ASC->AddGameplayTag(AddToTag);
	}
}

void UWKAnimNotifyState_AddGameplayTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (UWKAbilitySystemComponent* ASC = UWKGameplayStatics::GetWKAbilitySystemComponent(MeshComp->GetOwner()))
	{
		ASC->RemoveGameplayTag(AddToTag);
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
