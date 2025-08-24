// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimInstanceBase.h"

#include "BlackWK/Character/WKCharacterBase.h"

void UWKAnimInstanceBase::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
}

void UWKAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AWKCharacterBase* OwnerCharacter = Cast<AWKCharacterBase>(TryGetPawnOwner()))
	{
		OwnerWK = OwnerCharacter;
	}
}

void UWKAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	UpdateCharacterInfo();
}

void UWKAnimInstanceBase::UpdateCharacterInfo()
{
	if (!OwnerWK.IsValid())
	{
		return;
	}

	if (IWKCharacterAnimInfoInterface* OwnerInterface = Cast<IWKCharacterAnimInfoInterface>(OwnerWK.Get()))
	{
		FWKEssentialValue EssentialValue = OwnerInterface->GetEssentialValues();
		Velocity = EssentialValue.Velocity;
		Acceleration = EssentialValue.Acceleration;
		MovementInput = EssentialValue.MovementInput;
		bIsMoving = EssentialValue.bIsMoving;
		bHasMovementInput = EssentialValue.bHasMovementInput;
		Speed = EssentialValue.Speed;
		MovementInputAmount = EssentialValue.MovementInputAmount;
		AimingRotation = EssentialValue.AimingRotation;

		FWKCharacterState CharacterState = OwnerInterface->GetCharacterState();
		MovementState = CharacterState.MovementState;
		RotationMode = CharacterState.RotationMode;
	}
}