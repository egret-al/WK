// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKPlayerCharacterBase.h"
#include "BlackWK/AbilitySystem/WKAbilitySourceInterface.h"
#include "WuKongPlayerCharacter.generated.h"

UCLASS()
class BLACKWK_API AWuKongPlayerCharacter : public AWKPlayerCharacterBase, public IWKAbilitySourceInterface
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
	virtual FWKCharacterState GetCharacterState() override;

	void OnCharacterMovementModeChanged(EMovementMode PrevMovementMode, EMovementMode NewMovementMode, uint8 PrevCustomMode, uint8 NewCustomMode);
	void SetMovementState(EWKMovementState NewMovementState);
	void OnMovementStateChanged(EWKMovementState NewMovementState);

protected:
	EWKMovementState MovementState = EWKMovementState::None;
};
