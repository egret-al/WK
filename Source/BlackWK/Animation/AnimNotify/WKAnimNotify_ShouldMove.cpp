// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotify_ShouldMove.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "BlackWK/AbilitySystem/WKGameplayTags.h"

void UWKAnimNotify_ShouldMove::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* Actor = MeshComp->GetOwner();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, WKGameplayTags::Gameplay_Message_ShouldMove, FGameplayEventData());
}
