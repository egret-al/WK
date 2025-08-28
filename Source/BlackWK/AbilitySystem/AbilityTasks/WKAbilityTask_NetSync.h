// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WKAbilityTask_NetSync.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAbilityTask_NetSync : public UAbilityTask
{
	GENERATED_BODY()

public:
	virtual void OnDestroy(bool bInOwnerFinished) override;

protected:
	FGuid NetSyncGuid;
};
