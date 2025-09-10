// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotify_SendGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"

void UWKAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
}
