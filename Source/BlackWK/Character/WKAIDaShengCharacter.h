// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAICharacterBase.h"
#include "BlackWK/AbilitySystem/WKAbilitySourceInterface.h"
#include "WKAIDaShengCharacter.generated.h"

UCLASS()
class BLACKWK_API AWKAIDaShengCharacter : public AWKAICharacterBase, public IWKAbilitySourceInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
};
