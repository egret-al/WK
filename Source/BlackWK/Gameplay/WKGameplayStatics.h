// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "WKGameplayStatics.generated.h"

class UWKAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class BLACKWK_API UWKGameplayStatics : public UGameplayStatics
{
	GENERATED_BODY()

public:
	static bool WorldIsGame(const UObject* WorldContextObj);

	UFUNCTION(BlueprintPure)
	static UWKAbilitySystemComponent* GetWKAbilitySystemComponent(AActor* InActor);
};
