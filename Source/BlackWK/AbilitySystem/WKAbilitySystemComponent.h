// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DataAssets/WKGameplayAbilityDataAsset.h"
#include "WKAbilitySystemComponent.generated.h"

struct FWKGameplayAbilityPriorityInfo;
class UWKGameplayAbility;

DECLARE_MULTICAST_DELEGATE_OneParam(FWKAbilityInputDelegate, int32)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWKGameplayAbilityPriorityDelegate, float, OldPriority, float, NewPriority);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLACKWK_API UWKAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UWKAbilitySystemComponent();
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "WKAbilitySystemComponent")
	bool HasMatchingGameplayTagExactly(FGameplayTag TagToCheck) const;
	
	void ProcessAbilityInput();
	void ClearAbilityInput();

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	FSimpleMulticastDelegate& WKAbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::Type EventType, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey);

	UFUNCTION(BlueprintCallable)
	void UpdateCurrentPriorityAbility(UWKGameplayAbility* RequestAbility, const FWKGameplayAbilityPriorityInfo& PriorityInfo);
	
	UFUNCTION(BlueprintPure)
	UWKGameplayAbility* GetCurrentPriorityAbility() const;

	UFUNCTION(BlueprintPure)
	FWKGameplayAbilityPriorityInfo GetCurrentPriorityInfo() const;
	
	UFUNCTION()
	void ApplyPriorityBlockAndCancel(UWKGameplayAbility* RequestAbility, const FWKGameplayAbilityPriorityInfo& PriorityInfo, bool bEndAbility);

	void SetCurrentPriorityAbilityTime(float Time) { CurrentPriorityAbilityTime = Time; }

	UFUNCTION(BlueprintPure)
	float GetCurrentPriorityAbilityTime() const { return CurrentPriorityAbilityTime; }

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

	UPROPERTY(BlueprintAssignable)
	FWKGameplayAbilityPriorityDelegate OnAbilityPriorityUpdateDelegate;

protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

private:
	UPROPERTY()
	float LastPriorityAbilityTime;

	UPROPERTY()
	float CurrentPriorityAbilityTime;
	
	UPROPERTY()
	TObjectPtr<UWKGameplayAbility> LastPriorityAbility;
	
	// 当前激活中的优先级GA，没有时为空
	UPROPERTY()
	TObjectPtr<UWKGameplayAbility> CurrentPriorityAbility;

	// 优先级打断相关信息
	UPROPERTY()
	FWKGameplayAbilityPriorityInfo CurrentPriorityInfo;
};
