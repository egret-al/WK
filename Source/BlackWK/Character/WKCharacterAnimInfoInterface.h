// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKCharacterTypes.h"
#include "UObject/Interface.h"
#include "WKCharacterAnimInfoInterface.generated.h"

struct FWKEssentialValue;

UINTERFACE()
class UWKCharacterAnimInfoInterface : public UInterface
{
	GENERATED_BODY()
};

class BLACKWK_API IWKCharacterAnimInfoInterface
{
	GENERATED_BODY()

public:
	virtual FWKEssentialValue GetEssentialValues() { return FWKEssentialValue(); }
	virtual FWKCharacterState GetCharacterState() { return FWKCharacterState(); }

	USkeletalMeshComponent* GetWeaponMeshComponent() { return nullptr; }
};
