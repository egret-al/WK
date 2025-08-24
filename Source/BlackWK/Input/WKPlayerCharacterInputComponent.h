// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"

#include "WKPlayerCharacterInputComponent.generated.h"

class UWKInputConfig;
class UInputMappingContext;

USTRUCT()
struct FInputMappingContextAndPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	// Higher priority input mappings will be prioritized over mappings with a lower priority.
	UPROPERTY(EditAnywhere, Category="Input")
	int32 Priority = 0;
	
	/** If true, then this mapping context will be registered with the settings when this game feature action is registered. */
	UPROPERTY(EditAnywhere, Category="Input")
	bool bRegisterWithSettings = true;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLACKWK_API UWKPlayerCharacterInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWKPlayerCharacterInputComponent(const FObjectInitializer& ObjectInitializer);
	static UWKPlayerCharacterInputComponent* FindPlayerCharacterInputComponent(const AActor* InActor);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

protected:
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "WKConfig|Input")
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;

	bool bReadyToBindInputs;
};
