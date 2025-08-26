// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BlackWK/Data/WKGameDataAsset.h"
#include "UObject/Object.h"
#include "WKGameplayAbilityDataAsset.generated.h"

/**
 *									优先级打断系统概述
 *
 * 文档: http://confluence.digi-sky.com/pages/viewpage.action?pageId=57902086
 *
 *
 * 相关逻辑写于：
 *
 * DSGameplayAbility:
 *		CanActivateAbility			 - 根据优先级信息，检查能否激活
 *		PreActivate					 - 激活前在Component上更新优先级信息，并调用屏蔽及打断等操作
 *
 * DSAbilitySystemComponent:
 *		ApplyPriorityBlockAndCancel  - 设置优先级信息到Component，并应用一些屏蔽及打断逻辑
 *		UpdateCurrentPriorityInfo	 - 对外接口，更新优先级信息，常用于动态修改优先级信息
 *
 * DSCharacter:
 *		Crouch						 - 重写基类Crouch, 加入能否下蹲的检测
 *		MoveForward & MoveRight		 - 加入能否移动的检测
 *
 * UDSAnimNotify_UpdatePriorityInfo
 *		Notify						 - AnimNotify，动画打点，动态修改优先级
 *
 *
 * 当一个技能激活时，常规的执行顺序：
 * CanActivateAbility --> PreActive --> ApplyPriorityBlockAndCancel --> ActiveAbility
 *
 *
 */
USTRUCT(BlueprintType)
struct FWKGameplayAbilityPriorityInfo
{
	GENERATED_BODY()

public:
	FWKGameplayAbilityPriorityInfo();
	
	// 初始优先级
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WKConfig|Priority")
	float Priority;
	
	// 是否可以移动
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WKConfig|Priority", meta = (EditCondition = "Priority >= 0 && bCheckPriorityActivateOnly == false"))
	bool bCanMove;
	
	// 只根据优先级检查是否可以激活，但激活时不纳入系统管理
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig|Priority", meta = (EditCondition = "Priority >= 0"))
	bool bCheckPriorityActivateOnly;
	
	// 不纳入优先级系统管理的时候是否可以打断前一个技能
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig|Priority", meta = (EditCondition = "bCheckPriorityActivateOnly == true", EditConditionHides))
	bool bCancelLastActiveAbility;
	
	//激活时忽略优先级判断，但是会被更高优先级的打断
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig|Priority", meta = (EditCondition = "Priority >= 0", EditConditionHides))
	bool bForceActivateAbility;
};

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKGameplayAbilityDataAsset : public UWKGameDataAsset
{
	GENERATED_BODY()

public:
	// 技能优先级配置
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig|Priority")
	FWKGameplayAbilityPriorityInfo PriorityInfo;

	// 技能标识
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig|Priority")
	FGameplayTag AbilityTag;
};
