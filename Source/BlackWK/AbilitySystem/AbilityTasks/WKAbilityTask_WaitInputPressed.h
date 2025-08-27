// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "BlackWK/AbilitySystem/Abilities/WKAbilityTypes.h"
#include "WKAbilityTask_WaitInputPressed.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWKInputPressedDelegate, float, TimeHeld);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWKInputPressedPayloadDelegate, FVector, Payload, int32, InputID);

/**
 *	Waits until the input is released from activating an ability. Clients will replicate a 'release input' event to the server, but not the exact time it was held locally.
 *	We expect server to execute this task in parallel and keep its own time.
 */
UCLASS()
class BLACKWK_API UWKAbilityTask_WaitInputPressed : public UAbilityTask
{
	GENERATED_BODY()

public:
	UWKAbilityTask_WaitInputPressed(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/** 如果payload参数不为vector则为绑定的InputID,存在PayLoad的X中；如果一个GA里有多个按键监听，一定要把bWithPayLoad为true且bPayLoadIsVector为false，因为这样会把vector当按下的按键参数作为判定 */
	UFUNCTION(BlueprintCallable, Category="DSAbility|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UWKAbilityTask_WaitInputPressed* WKWaitPressedWithInput(UGameplayAbility* OwningAbility, EWKGameplayAbilityInputBinds InputID, bool bOnlyTriggerOnce = true, bool bWithPayload = false, bool bPayloadIsVector = true, FVector Payload = FVector(0, 0, 0));

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

public:
	UPROPERTY(BlueprintAssignable)
	FWKInputPressedPayloadDelegate OnPressedWithPayload;
	
	UPROPERTY(BlueprintAssignable)
	FWKInputPressedDelegate	OnPressed;

protected:
	UFUNCTION()
	void OnPressedCallback();
	
protected:
	int32 BindInputID;
	float StartTime;
	FDelegateHandle DelegateHandle;
	bool bOnlyTriggerOnce;

	UPROPERTY(BlueprintReadWrite)
	bool bWithPayload;

	// 如果payload参数不为vector则为绑定的InputID
	UPROPERTY(BlueprintReadWrite)
	bool bPayloadIsVector;
	
	UPROPERTY(BlueprintReadWrite)
	FVector Payload;
};
