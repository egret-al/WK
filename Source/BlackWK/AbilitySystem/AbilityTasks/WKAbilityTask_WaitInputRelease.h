// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WKAbilityTask_WaitInputRelease.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWKInputReleaseDelegate, float, TimeHeld);

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAbilityTask_WaitInputRelease : public UAbilityTask
{
	GENERATED_BODY()

public:
	UWKAbilityTask_WaitInputRelease(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/** Wait until the user releases the input button for this ability's activation. Returns time from hitting this node, till release. Will return 0 if input was already released. */
	UFUNCTION(BlueprintCallable, Category="DSAbility|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UWKAbilityTask_WaitInputRelease* WKWaitReleaseWithInput(UGameplayAbility* OwningAbility, EWKGameplayAbilityInputBinds InputID, bool bOnlyTriggerOnce = true);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	
public:
	UPROPERTY(BlueprintAssignable)
	FWKInputReleaseDelegate	OnRelease;

protected:
	UFUNCTION()
	void OnReleaseCallback();

protected:
	int32 BindInputID;
	float StartTime;
	FDelegateHandle DelegateHandle;
	bool bOnlyTriggerOnce;
};
