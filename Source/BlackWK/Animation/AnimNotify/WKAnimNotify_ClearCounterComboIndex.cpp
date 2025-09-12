// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotify_ClearCounterComboIndex.h"

#include "BlackWK/Character/WKCharacterBase.h"

void UWKAnimNotify_ClearCounterComboIndex::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AWKCharacterBase* MeshOwner = Cast<AWKCharacterBase>(MeshComp->GetOwner()))
	{
		MeshOwner->ClearCounterComboIndex();
	}
}

FString UWKAnimNotify_ClearCounterComboIndex::GetNotifyName_Implementation() const
{
	return FString(TEXT("ClearCounterComboIndex"));
}
