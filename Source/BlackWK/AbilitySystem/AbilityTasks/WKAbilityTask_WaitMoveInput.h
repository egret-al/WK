// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WKAbilityTask_WaitMoveInput.generated.h"

class UWKPlayerCharacterInputComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWKWaitMoveInputDelegate, FVector2D, MoveInput);

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAbilityTask_WaitMoveInput : public UAbilityTask
{
	GENERATED_BODY()

public:
	UWKAbilityTask_WaitMoveInput(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitMoveInput", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UWKAbilityTask_WaitMoveInput* WaitMoveInput(UGameplayAbility* OwningAbility,bool OnlyTriggerOnce=true);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	
public:
	UPROPERTY(BlueprintAssignable)
	FWKWaitMoveInputDelegate OnMoveInput;

protected:
	UPROPERTY()
	TObjectPtr<UWKPlayerCharacterInputComponent> InputComponent;

	UPROPERTY()
	bool bOnlyTriggerOnce;
};
