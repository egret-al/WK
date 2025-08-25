// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WKAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FWKAbilityInputDelegate, int32)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLACKWK_API UWKAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UWKAbilitySystemComponent();
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;


	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "WKAbilitySystemComponent")
	bool HasMatchingGameplayTagExactly(FGameplayTag TagToCheck) const;
	
	void ProcessAbilityInput();
	void ClearAbilityInput();

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	FSimpleMulticastDelegate& WKAbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::Type EventType, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey);
	
protected:
	/** 尝试在生成时就去激活可以激活的GA */
	void TryActivateAbilitiesOnSpawn();

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	virtual void AbilityLocalInputPressed(int32 InputID) override;
	virtual void AbilityLocalInputReleased(int32 InputID) override;

public:
	FWKAbilityInputDelegate OnAbilityInputPressedDelegate;
	FWKAbilityInputDelegate OnAbilityInputReleaseDelegate;

protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
};
