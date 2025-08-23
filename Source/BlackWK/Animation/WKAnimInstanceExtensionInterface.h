// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WKAnimInstanceExtensionInterface.generated.h"

class AWKCharacterBase;

UINTERFACE()
class UWKAnimInstanceExtensionInterface : public UInterface
{
	GENERATED_BODY()
};

class BLACKWK_API IWKAnimInstanceExtensionInterface
{
	GENERATED_BODY()

public:
	virtual void ModifyRootMotionTransform(FTransform& InoutTransform) {}

	virtual void OnEnterLockTarget(AWKCharacterBase* LockableTarget) {}
	virtual void OnExitLockTarget() {}
};
