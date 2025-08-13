// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKCharacterBase.h"
#include "WKAICharacterBase.generated.h"

class UWidgetComponent;
struct FOnAttributeChangeData;
class UWKFloatingStatusBarWidget;

UCLASS()
class BLACKWK_API AWKAICharacterBase : public AWKCharacterBase
{
	GENERATED_BODY()

public:
	AWKAICharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

protected:
	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	// Tag change callbacks
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

protected:
	// Actual hard pointer to AbilitySystemComponent
	UPROPERTY()
	TObjectPtr<UWKAbilitySystemComponent> HardRefAbilitySystemComponent;

	// Actual hard pointer to AttributeSetBase
	UPROPERTY()
	TObjectPtr<UWKAttributeSetBase> HardRefAttributeSetBase;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASDocumentation|UI")
	TSubclassOf<UWKFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
	TObjectPtr<UWKFloatingStatusBarWidget> UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASDocumentation|UI")
	TObjectPtr<UWidgetComponent> UIFloatingStatusBarComponent;

	FDelegateHandle HealthChangedDelegateHandle;
};
