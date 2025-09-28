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
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Condition_ResoluteCounterflow, "Gameplay.Condition.ResoluteCounterflow", "需要有该标签，才能激活识破切手技");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Message_Damage_Hit, "Gameplay.Message.Damage.Hit", "命中敌人时发送的事件");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Message_Damage_BeHitEnd, "Gameplay.Message.Damage.BeHitEnd", "被命中时播放的受击动画，收到这个事件后，可以执行随机结束的逻辑");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Message_ShouldMove, "Gameplay.Message.ShouldMove", "GA播放动画的过程中，执行到了可提前结束GA的移动指令");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Message_Attack_Detection, "Gameplay.Message.Attack.Detection", "被攻击检测到时，被检测对象会收到这个消息，不论是否有无敌状态");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Event_BeHit, "Gameplay.Event.BeHit", "受击GA，通过GameplayEvent激活");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Event_PerfectDodge, "Gameplay.Event.PerfectDodge", "成功完美闪避，通过GameplayEvent激活");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_EventData_DodgeF, "Gameplay.EventData.DodgeF", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_EventData_DodgeB, "Gameplay.EventData.DodgeB", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_EventData_DodgeL, "Gameplay.EventData.DodgeL", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_EventData_DodgeR, "Gameplay.EventData.DodgeR", "");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill01, "Ability.DaSheng.Skill.Skill01", "大圣激活技能1的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill02, "Ability.DaSheng.Skill.Skill02", "大圣激活技能2的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill03, "Ability.DaSheng.Skill.Skill03", "大圣激活技能3的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill04, "Ability.DaSheng.Skill.Skill04", "大圣激活技能4的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill05, "Ability.DaSheng.Skill.Skill05", "大圣激活技能5的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill06, "Ability.DaSheng.Skill.Skill06", "大圣激活技能6的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill07, "Ability.DaSheng.Skill.Skill07", "大圣激活技能7的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill08, "Ability.DaSheng.Skill.Skill08", "大圣激活技能8的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill09, "Ability.DaSheng.Skill.Skill09", "大圣激活技能9的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill10, "Ability.DaSheng.Skill.Skill10", "大圣激活技能10的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill11, "Ability.DaSheng.Skill.Skill11", "大圣激活技能11的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill12, "Ability.DaSheng.Skill.Skill12", "大圣激活技能12的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill13, "Ability.DaSheng.Skill.Skill13", "大圣激活技能13的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill14, "Ability.DaSheng.Skill.Skill14", "大圣激活技能14的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill15, "Ability.DaSheng.Skill.Skill15", "大圣激活技能15的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill16, "Ability.DaSheng.Skill.Skill16", "大圣激活技能16的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill17, "Ability.DaSheng.Skill.Skill17", "大圣激活技能17的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill18, "Ability.DaSheng.Skill.Skill18", "大圣激活技能18的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill19, "Ability.DaSheng.Skill.Skill19", "大圣激活技能19的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill20, "Ability.DaSheng.Skill.Skill20", "大圣激活技能20的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill21, "Ability.DaSheng.Skill.Skill21", "大圣激活技能21的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill22, "Ability.DaSheng.Skill.Skill22", "大圣激活技能22的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_DaSheng_Skill_Skill23, "Ability.DaSheng.Skill.Skill23", "大圣激活技能23的Tag");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_WuKong_Skill_Charge, "Ability.WuKong.Skill.Charge", "悟空蓄力技");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_WuKong_Skill_ResoluteCounterflow, "Ability.WuKong.Skill.ResoluteCounterflow", "悟空蓄力技");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_WuKong_Movement_Dodge, "Ability.WuKong.Movement.Dodge", "悟空激活闪避的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_WuKong_Movement_PerfectDodge, "Ability.WuKong.Movement.PerfectDodge", "悟空激活完美闪避的Tag");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Amount_StickEnergy, "SetByCaller.Amount.StickEnergy", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Amount_BaseDamage, "SetByCaller.Amount.BaseDamage", "");

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
