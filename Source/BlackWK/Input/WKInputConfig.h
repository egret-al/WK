// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "WKInputConfig.generated.h"

class UInputAction;

/**
 * IA与Tag的映射
 */
USTRUCT(BlueprintType)
struct FWKInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsTrigger = false;
};

/**
 * 输入配置映射
 */
UCLASS(BlueprintType, Const)
class BLACKWK_API UWKInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UWKInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable)
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	/** IA和Tag的映射，必须手动绑定 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FWKInputAction> NativeInputActions;

	/** IA和Tag的映射，自动绑定到具有这个输入Tag的GA上 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FWKInputAction> AbilityInputActions;
};