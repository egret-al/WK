// Fill out your copyright notice in the Description page of Project Settings.


#include "WuKongPlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

void AWuKongPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AWuKongPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	OnCharacterMovementModeChanged(PrevMovementMode, GetCharacterMovement()->MovementMode, PreviousCustomMode, GetCharacterMovement()->CustomMovementMode);
}

FWKCharacterState AWuKongPlayerCharacter::GetCharacterState()
{
	FWKCharacterState CharacterState;
	CharacterState.MovementState = MovementState;
	CharacterState.RotationMode = RotationMode;
	return CharacterState;
}

void AWuKongPlayerCharacter::OnCharacterMovementModeChanged(EMovementMode PrevMovementMode, EMovementMode NewMovementMode, uint8 PrevCustomMode, uint8 NewCustomMode)
{
	if (NewMovementMode == MOVE_Walking || NewMovementMode == MOVE_NavWalking)
	{
		SetMovementState(EWKMovementState::Grounded);
	}
	else if (NewMovementMode == MOVE_Falling)
	{
		SetMovementState(EWKMovementState::InAir);
	}
}

void AWuKongPlayerCharacter::SetMovementState(EWKMovementState NewMovementState)
{
	if (MovementState != NewMovementState)
	{
		OnMovementStateChanged(NewMovementState);
	}
}

void AWuKongPlayerCharacter::OnMovementStateChanged(EWKMovementState NewMovementState)
{
	MovementState = NewMovementState;
}
