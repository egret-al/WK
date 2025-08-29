// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities.h"

#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

void AddNativeTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}