// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WKAbilityTask_TickTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWKTickTaskDelegate, float, DeltaTime);

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAbilityTask_TickTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	UWKAbilityTask_TickTask(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/** 参数为一秒多少帧的频率走，如果为0默认为一秒10次tick，-1为最大帧率走 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UWKAbilityTask_TickTask* TickTask(UGameplayAbility* OwningAbility, const float TickFrequency);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

public:
	UPROPERTY(BlueprintAssignable)
	FWKTickTaskDelegate OnTickDelegate;
	
private:
	UPROPERTY()
	float TickTimeInterval;

	UPROPERTY()
	bool bEnableTickInterval;
	
	UPROPERTY(Transient)
	float LastTickTime;
};
