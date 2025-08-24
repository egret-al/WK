// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace WKGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Gameplay_AbilityInputBlocked);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	
	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
}