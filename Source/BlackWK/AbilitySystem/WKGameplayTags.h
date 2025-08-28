// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace WKGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Gameplay_AbilityInputBlocked);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_State_Invincible);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_State_IronBody);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Message_Damage_Hit);
	
	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
}