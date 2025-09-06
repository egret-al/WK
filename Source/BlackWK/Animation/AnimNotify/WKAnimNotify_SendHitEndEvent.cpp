// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotify_SendHitEndEvent.h"

#include "AbilitySystemBlueprintLibrary.h"

void UWKAnimNotify_SendHitEndEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	FGameplayEventData EventData;
	EventData.EventMagnitude = RandomDelayTime;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventData);
}
