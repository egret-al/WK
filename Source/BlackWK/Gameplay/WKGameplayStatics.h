// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "WKGameplayStatics.generated.h"

class AWKPlayerController;
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

	/** 获取本地客户端的PlayerController */
	UFUNCTION(BlueprintPure, Category = "WKGame", meta = (WorldContext = "WorldContextObject"))
	static APlayerController* GetLocalPlayerController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "WKGame", meta = (WorldContext = "WorldContextObject"))
	static TArray<AWKPlayerController*> GetAllPlayerController(const UObject* WorldContextObject);
};
