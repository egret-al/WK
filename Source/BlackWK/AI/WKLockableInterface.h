// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WKLockableInterface.generated.h"

UINTERFACE()
class UWKLockableInterface : public UInterface
{
	GENERATED_BODY()
};

class BLACKWK_API IWKLockableInterface
{
	GENERATED_BODY()

public:
	/** 获取可锁定目标的锁定点 */
	UFUNCTION(BlueprintNativeEvent)
	FVector GetLockLocation();
};
