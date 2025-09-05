// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotifyState_CloseWeaponCollision.h"

void UWKAnimNotifyState_CloseWeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	for (FName BoneName : BoneNames)
	{
		if (FBodyInstance* BodyInstance = MeshComp->GetBodyInstance(BoneName))
		{
			BodyInstance->SetShapeCollisionEnabled(0, ECollisionEnabled::Type::NoCollision);
		}
	}
}

void UWKAnimNotifyState_CloseWeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	for (FName BoneName : BoneNames)
	{
		if (FBodyInstance* BodyInstance = MeshComp->GetBodyInstance(BoneName))
		{
			BodyInstance->SetShapeCollisionEnabled(0, ECollisionEnabled::Type::QueryAndPhysics);
		}
	}
	
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
