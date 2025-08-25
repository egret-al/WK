// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotify_ClearMeleeComboIndex.h"

#include "BlackWK/Character/WKCharacterBase.h"

void UWKAnimNotify_ClearMeleeComboIndex::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AWKCharacterBase* MeshOwner = Cast<AWKCharacterBase>(MeshComp->GetOwner()))
	{
		MeshOwner->ClearMeleeComboIndex();
	}
}
