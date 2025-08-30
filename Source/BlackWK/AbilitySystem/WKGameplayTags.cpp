// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayTags.h"

namespace WKGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Gameplay_AbilityInputBlocked, "InputTag.Gameplay.AbilityInputBlocked", "阻止GA输入");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "InputTag.Move", "角色基本移动");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Mouse, "InputTag.Look.Mouse", "控制视角移动");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_State_Invincible, "Gameplay.State.Invincible", "无敌状态");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_State_IronBody, "Gameplay.State.IronBody", "霸体状态");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_State_Input_BlockMove, "Gameplay.State.Input.BlockMove", "阻塞输入移动");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Message_Damage_Hit, "Gameplay.Message.Damage.Hit", "命中敌人时发送的事件");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Message_ShouldMove, "Gameplay.Message.ShouldMove", "GA播放动画的过程中，执行到了可提前结束GA的移动指令");

	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					Tag = TestTag;
					break;
				}
			}
		}
		return Tag;
	}
}
