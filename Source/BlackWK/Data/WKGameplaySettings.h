// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WKGameplaySettings.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS(Config = WKGameplaySettings, DefaultConfig, meta = (DisplayName = "WKGameplaySettings"))
class BLACKWK_API UWKGameplaySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UWKGameplaySettings();

	UFUNCTION(BlueprintPure, Category = "Settings")
	static UWKGameplaySettings* GetGameplaySettings();

	virtual FName GetCategoryName() const override;
	virtual FText GetSectionText() const override;

public:
	// AI技能信息表
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "DataTable|AI")
	TSoftObjectPtr<UDataTable> SkillTable;
};
