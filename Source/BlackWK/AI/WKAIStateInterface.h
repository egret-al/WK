// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WKAIStateInterface.generated.h"

class AWKCharacterBase;

UINTERFACE()
class UWKAIStateInterface : public UInterface
{
	GENERATED_BODY()
};

class BLACKWK_API IWKAIStateInterface
{
	GENERATED_BODY()

public:
	virtual bool HasSawEnemyTarget() { return false; }
	virtual AWKCharacterBase* GetCurrentTarget() { return nullptr; }
};
