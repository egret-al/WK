// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotify_RemoveGameplayTag.h"

#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/Gameplay/WKGameplayStatics.h"

void UWKAnimNotify_RemoveGameplayTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (UWKAbilitySystemComponent* ASC = UWKGameplayStatics::GetWKAbilitySystemComponent(MeshComp->GetOwner()))
	{
		ASC->RemoveGameplayTag(RemoveToTag);
	}
}

FString UWKAnimNotify_RemoveGameplayTag::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Remove: %s"), *RemoveToTag.ToString()); 
}
