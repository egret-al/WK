// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WKAbilityTask_WaitGameplayTags.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWKWaitGameplayTagsDelegate, const FGameplayTag&, InTag);

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAbilityTask_WaitGameplayTags : public UAbilityTask
{
	GENERATED_BODY()

public:
	UWKAbilityTask_WaitGameplayTags(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityIsEnding) override;

	UFUNCTION()
	void GameplayTagCallback(const FGameplayTag InTag, int32 NewCount);
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UWKAbilityTask_WaitGameplayTags* WaitGameplayTags(UGameplayAbility* OwningAbility, FGameplayTagContainer Container, bool ActivateCheck = true, bool bActivateCheckOnlyCallAdd = false);

public:
	UPROPERTY(BlueprintAssignable)
	FWKWaitGameplayTagsDelegate Added;

	UPROPERTY(BlueprintAssignable)
	FWKWaitGameplayTagsDelegate Removed;

	UPROPERTY(BlueprintAssignable)
	FWKWaitGameplayTagsDelegate AllRemoved;

	FGameplayTagContainer TagContainer;

protected:
	TMap<FGameplayTag, FDelegateHandle> DelegateHandles;

	TArray<FGameplayTag> WaitTags;

	//激活检查
	UPROPERTY()
	bool ActivateCheck;

	//激活检查只调用Add
	UPROPERTY()
	bool bActivateCheckOnlyCallAdd;
};
