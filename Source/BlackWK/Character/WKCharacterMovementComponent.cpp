// Fill out your copyright notice in the Description page of Project Settings.


#include "WKCharacterMovementComponent.h"

#include "BlackWK/Animation/WKAnimInstanceExtensionInterface.h"
#include "GameFramework/Character.h"

static FTransform FixRootMotionTransformStatic(const FTransform& InTransform, UCharacterMovementComponent* InMovement, float DeltaSeconds)
{
	FTransform OutTransform = InTransform;
	if (ACharacter* Character = InMovement->GetCharacterOwner())
	{
		if (IWKAnimInstanceExtensionInterface* AnimInst = Cast<IWKAnimInstanceExtensionInterface>(Character->GetMesh()->GetAnimInstance()))
		{
			AnimInst->ModifyRootMotionTransform(OutTransform);
		}
	}
	return OutTransform;
}

UWKCharacterMovementComponent::UWKCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProcessRootMotionPostConvertToWorld.BindStatic(FixRootMotionTransformStatic);
}
