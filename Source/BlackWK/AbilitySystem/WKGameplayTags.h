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
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_State_Input_BlockMove);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Message_Damage_Hit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Message_ShouldMove);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Event_BeHit);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_DaSheng_Skill_Skill01);
	
	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
}