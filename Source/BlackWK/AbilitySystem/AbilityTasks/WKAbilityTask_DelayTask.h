// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WKAbilityTask_DelayTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWKDelayTaskDelegate);

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAbilityTask_DelayTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	UWKAbilityTask_DelayTask(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UWKAbilityTask_DelayTask* DelayTask(UGameplayAbility* OwningAbility, const float DelayTime, bool bRepeated = false);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

public:
	UPROPERTY(BlueprintAssignable)
	FWKDelayTaskDelegate OnDelayDelegate;

private:
	UPROPERTY()
	bool bAllowRepeated;

	UPROPERTY()
	float DelayTime;

	UPROPERTY(Transient)
	float CurrentTime;
};
