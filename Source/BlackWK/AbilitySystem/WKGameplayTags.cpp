// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayTags.h"

namespace WKGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Gameplay_AbilityInputBlocked, "InputTag.Gameplay.AbilityInputBlocked", "阻止GA输入");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "InputTag.Move", "角色基本移动");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Mouse, "InputTag.Look.Mouse", "控制视角移动");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_State_Invincible, "Gameplay.State.Invincible", "无敌状态");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_State_IronBody, "Gameplay.State.IronBody", "霸体状态");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_State_Dodging, "Gameplay.State.Dodging", "正在闪避的状态");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_State_Input_BlockMove, "Gameplay.State.Input.BlockMove", "阻塞输入移动");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Message_Damage_Hit, "Gameplay.Message.Damage.Hit", "命中敌人时发送的事件");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Message_ShouldMove, "Gameplay.Message.ShouldMove", "GA播放动画的过程中，执行到了可提前结束GA的移动指令");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Message_Attack_Detection, "Gameplay.Message.Attack.Detection", "被攻击检测到时，被检测对象会收到这个消息，不论是否有无敌状态");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Event_BeHit, "Gameplay.Event.BeHit", "受击GA，通过GameplayEvent激活");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Event_PerfectDodge, "Gameplay.Event.PerfectDodge", "成功完美闪避，通过GameplayEvent激活");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_EventData_DodgeF, "Gameplay.EventData.DodgeF", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_EventData_DodgeB, "Gameplay.EventData.DodgeB", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_EventData_DodgeL, "Gameplay.EventData.DodgeL", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_EventData_DodgeR, "Gameplay.EventData.DodgeR", "");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill01, "Ability.DaSheng.Skill.Skill01", "大圣激活技能1的Tag");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_WuKong_Movement_Dodge, "Ability.WuKong.Movement.Dodge", "悟空激活闪避的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_WuKong_Movement_PerfectDodge, "Ability.WuKong.Movement.PerfectDodge", "悟空激活完美闪避的Tag");

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
