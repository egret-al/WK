// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimNotifyState.h"
#include "UObject/Object.h"
#include "WKAnimNotifyState_RootMotionWarping.generated.h"

#define SERIALIZE_BY_CONDITION(Flag, SerializeOp, ResetOp) \
if (RepBits & (1 << (Flag)))                          \
{                                                     \
	SerializeOp;                                      \
}                                                     \
else if (Ar.IsLoading())                              \
{                                                     \
	ResetOp;                                          \
}

UENUM(BlueprintType)
enum class EWKRotateOrientation: uint8
{
	/** 最小角度方向 */
	MinAngle = 0,
	/** 顺时针方向 */
	Clockwise = 1,
	/** 逆时针方向 */
	Anticlockwise = 2,
};

USTRUCT(BlueprintType)
struct FWKOverrideWarpingTargetActorParam
{
	GENERATED_BODY()

	/** 是否在过程中更新目标坐标 （若不勾选, 则只在动画通知一开始时获取一次目标坐标,运动过程中不再改变, 无论目标如何移动）*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bUpdateTargetPosition = true;

	/** 是否应用特异性增加追踪距离*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bApplySpecificity = false;
	
	/** 最大追踪距离 （负数代表不限制）（角色到目标点的距离）*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxDistance = -1.f;

	/** 没有找到TargetActor时，使用此距离缩放位移 （小于 0.f 时, 使用动画的原生位移距离;大于等于 0.f 时, 按配置值, 对原生动画进行缩放）*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WarpingDistanceIfTargetNotFound = -1.f;

	/** 追踪速度 （负数代表不限制）(不是角色速度，是目标点的速度)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ChasingSpeed = -1.f;

	/** 放弃追踪角度 （负数代表不限制）（角色朝向与目标点的夹角）*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RotationToAbort = -1.f;

	/** 旋转速率 （负数代表不限制）（角色转向的角速度）*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RotationRate = -1.f;

	/** 最大旋转角度（负数代表不限制）（角色自身转动超过这个角度之后不再追踪）*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RotationMax = -1.f;

	/** 旋转方向 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,meta = (EditCondition = "RotationRate > 0", EditConditionHides))
	EWKRotateOrientation RotationOrientation = EWKRotateOrientation::Clockwise;

	/** 最终坐标偏移*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector LocationOffset;

	/** 最终旋转偏移*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RotationOffset = 0.f;
};

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAnimNotifyState_RootMotionWarping : public UWKAnimNotifyState
{
	GENERATED_BODY()
};
