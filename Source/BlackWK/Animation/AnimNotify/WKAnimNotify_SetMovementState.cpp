// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotify_SetMovementState.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWKAnimNotify_SetMovementState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (ACharacter* OwnerCharacter = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(NewMovementMode.GetValue());
	}
}

FString UWKAnimNotify_SetMovementState::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("MovementMode: %s"), *UEnum::GetValueAsString(NewMovementMode));
}
