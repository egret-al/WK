// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WKAbilityTask_WaitStickEnergyHasMax.generated.h"

class UWKHealthComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWKAttributeMaxDelegate);

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAbilityTask_WaitStickEnergyHasMax : public UAbilityTask
{
	GENERATED_BODY()

public:
	UWKAbilityTask_WaitStickEnergyHasMax(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UWKAbilityTask_WaitStickEnergyHasMax* WaitStickEnergyHasMax(UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

protected:
	UFUNCTION()
	void OnStickEnergyChanged(UWKHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);
	
public:
	UPROPERTY(BlueprintAssignable)
	FWKAttributeMaxDelegate OnMaxDelegate;
};
