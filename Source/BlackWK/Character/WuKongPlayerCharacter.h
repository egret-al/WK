// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKPlayerCharacterBase.h"
#include "WuKongPlayerCharacter.generated.h"

UCLASS()
class BLACKWK_API AWuKongPlayerCharacter : public AWKPlayerCharacterBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
