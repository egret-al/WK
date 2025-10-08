// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplaySettings.h"

UWKGameplaySettings::UWKGameplaySettings()
{
}

UWKGameplaySettings* UWKGameplaySettings::GetGameplaySettings()
{
	static UWKGameplaySettings* Settings;
	if (!Settings)
	{
		Settings = GetMutableDefault<UWKGameplaySettings>();
	}
	return Settings;
}

FName UWKGameplaySettings::GetCategoryName() const
{
	return FName(TEXT("Project"));
}

FText UWKGameplaySettings::GetSectionText() const
{
	return NSLOCTEXT("WKGameplaySettings", "WKGameplaySettingsSection", "WK Gameplay Settings");
}
