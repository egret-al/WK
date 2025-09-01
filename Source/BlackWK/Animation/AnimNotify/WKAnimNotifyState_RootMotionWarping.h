// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimNotifyState.h"
#include "UObject/Object.h"
#include "WKAnimNotifyState_RootMotionWarping.generated.h"

enum class ERootMotionAccumulateMode : uint8;
class AWKCharacterBase;

UENUM(BlueprintType)
enum class EWKRootMotionWarpingType : uint8
{
	/** 距离缩放*/
	WarpingWithTranslation = 0,
	/** 旋转包装*/
	WarpingWithRotation = 1,
	/** 跟踪目标Actor*/
	WarpingWithTarget = 2,
	/** 给定的Location*/
	WarpingWithLocation = 3,
};

UCLASS(BlueprintType, Blueprintable, Abstract, EditInLineNew)
class UWKOverrideWarpingDistance : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	float GetOverrideWarpingDistance(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability);
	virtual float GetOverrideWarpingDistance_Implementation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability)
	PURE_VIRTUAL(UWKOverrideWarpingDistance::GetOverrideWarpingDistance_Implementation, return 0.f;);
};

UCLASS(BlueprintType, Blueprintable, Abstract, EditInLineNew)
class UWKOverrideWarpingDirection : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	FVector GetOverrideWarpingDirection(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability);
	virtual FVector GetOverrideWarpingDirection_Implementation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability)
	PURE_VIRTUAL(UWKOverrideWarpingDirection::GetOverrideWarpingDirection_Implementation, return FVector::ForwardVector;);
};

USTRUCT(BlueprintType)
struct FWKRootMotionWarping_Translation
{
	GENERATED_BODY()

public:
	
	/** 距离*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,meta = (UIMin = 0.0, ClampMin = 0.0))
	float WarpingDistance;

	/** 重载距离*/
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UWKOverrideWarpingDistance> OverrideWarpingDistance;

	/** 重载距离位移方向*/
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UWKOverrideWarpingDirection> OverrideWarpingDirection;
};

UENUM(BlueprintType)
enum class EWKRootMotionRotationType : uint8
{
	/** 基于自身*/
	BaseOnActor,
	/** 转向目标*/
	TurnToTarget,
	/** 基于玩家Controller（仅玩家可以使用）*/
	BaseOnController,
	/** 基于玩家移动输入（仅玩家可以使用）*/
	BaseOnInput,
};

UCLASS(BlueprintType,Blueprintable,Abstract, EditInLineNew)
class UWKOverrideWarpingRotation :public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	float GetOverrideWarpingRotation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability);
	virtual float GetOverrideWarpingRotation_Implementation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability)
	PURE_VIRTUAL(UWKOverrideWarpingDistance::GetOverrideWarpingRotation_Implementation, return 0.f;);
};

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

UCLASS(BlueprintType,Blueprintable,Abstract, EditInLineNew)
class UWKTargetCatchRule :public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* TargetSearch(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability);
	virtual AActor* TargetSearch_Implementation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability)
	PURE_VIRTUAL(UWKTargetCatchRule::TargetSearch_Implementation, return nullptr;);
};

USTRUCT(BlueprintType)
struct FWKRootMotionWarping_Rotation
{
	GENERATED_BODY()

public:
	/** 旋转类型 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EWKRootMotionRotationType RotationType;

	/** 旋转角度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,meta = (EditCondition = "RotationType == EWKRootMotionRotationType::BaseOnActor", EditConditionHides))
	float WarpingRotation;

	/** 重载旋转角度*/
	UPROPERTY(EditAnywhere, Instanced,meta = (EditCondition = "RotationType == EWKRootMotionRotationType::BaseOnActor", EditConditionHides))
	TObjectPtr<UWKOverrideWarpingRotation> OverrideWarpingRotation;

	/** 旋转方向 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,meta = (EditCondition = "RotationType == EWKRootMotionRotationType::TurnToTarget", EditConditionHides))
	EWKRotateOrientation RotationOrientation;
	
	/** 目标 */
	UPROPERTY(EditAnywhere, Instanced,meta = (EditCondition = "RotationType == EWKRootMotionRotationType::TurnToTarget", EditConditionHides))
	TObjectPtr<UWKTargetCatchRule> TargetCatchRule;
	
	/** 角速度 (默认-1则从通知开始到通知结束刚好转到面向目标的朝向)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,meta = (EditCondition = "RotationType == EWKRootMotionRotationType::TurnToTarget", EditConditionHides))
	float RotationAngleSpeed = -1.f;
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

UCLASS(BlueprintType, Blueprintable, Abstract, EditInLineNew)
class UWKOverrideWarpingTargetActor : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	FWKOverrideWarpingTargetActorParam GetOverrideWarpingTargetParam(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability);
	virtual FWKOverrideWarpingTargetActorParam GetOverrideWarpingTargetParam_Implementation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability)
	PURE_VIRTUAL(UWKOverrideWarpingDistance::GetOverrideWarpingRotation_Implementation, return FWKOverrideWarpingTargetActorParam(););
};

USTRUCT(BlueprintType)
struct FWKRootMotionWarping_TargetActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Instanced,BlueprintReadWrite)
	TObjectPtr<UWKTargetCatchRule> TargetCatchRule;

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
	EWKRotateOrientation RotationOrientation;

	/** 最终坐标偏移*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector LocationOffset;

	/** 最终旋转偏移*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RotationOffset;
	
	/** 重载距离位移方向*/
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UWKOverrideWarpingTargetActor> OverrideWarpingTargetActor;
};

UCLASS(BlueprintType, Blueprintable, Abstract, EditInLineNew)
class UWKCustomLocationWarping : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	FVector GetTargetLocation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability);
	virtual FVector GetTargetLocation_Implementation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability)
	PURE_VIRTUAL(UWKCustomLocationWarping::GetTargetLocation_Implementation, return FVector::ZeroVector;);
};

USTRUCT(BlueprintType)
struct FDSRootMotionWarping_Location
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UWKCustomLocationWarping> CustomTargetLocation;
};

UCLASS(DisplayName = "选取AI行为树的TargetActor")
class UWKTargetCatchRule_AIBlackboardTargetActor : public UWKTargetCatchRule
{
	GENERATED_BODY()

public:
	virtual AActor* TargetSearch_Implementation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability) override;
};

UCLASS(DisplayName = "基于玩家摄像机视口参数")
class UWKTargetCatchRule_PlayerCamera : public UWKTargetCatchRule
{
	GENERATED_BODY()

public:
	virtual AActor* TargetSearch_Implementation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability) override;

	/** 搜寻距离 */
	UPROPERTY(EditDefaultsOnly,meta = (UIMin = 0.0, ClampMin = 0.0))
	float SearchDistance = 1000.f;

	/** 准星距离系数*/
	UPROPERTY(EditDefaultsOnly)
	float SDMultiply = 40.f;

	/** 现实距离系数*/
	UPROPERTY(EditDefaultsOnly)
	float DMultiply = 100.f;

	/** 准星距离容差*/
	UPROPERTY(EditDefaultsOnly)
	float SDTolerance = 42.f;

	/** 只检测不在障碍物后面的*/
	UPROPERTY(EditDefaultsOnly)
	uint8 bCheckNoObstacle : 1 = true;

	bool bApplySpecificity = false;
};

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAnimNotifyState_RootMotionWarping : public UWKAnimNotifyState
{
	GENERATED_BODY()

public:
	UWKAnimNotifyState_RootMotionWarping();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	/** 和其他RootMotionSource的叠加方式*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERootMotionAccumulateMode AccumulateMode;

	/** 是否忽略Z轴输出*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIgnoreZAccumulate = true;

	/** 该段结束之后是否维持之前的速度*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMaintainVelocityWhenFinish;

	/** 蒙太奇开始混出（或者中途被打断）是否继续RootMotionWarping */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bContinueRootMotionWhenMontageStop;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWKRootMotionWarpingType MotionWarpingType;

	//是否Location转为水平的Translation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MotionWarpingType == EWKRootMotionWarpingType::WarpingWithLocation", EditConditionHides))
	bool bConvertLocationToTranslation = true;
	
	/** 当目标与自身存在高度差时，Z轴曲线*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (EditCondition = "(MotionWarpingType == EWKRootMotionWarpingType::WarpingWithTarget ||MotionWarpingType == EWKRootMotionWarpingType::WarpingWithLocation)&& bIgnoreZAccumulate == false",EditConditionHides))
	TObjectPtr<UCurveFloat> ZTrajectory;

	/** 保持原动画位移节奏进行缩放（如果不勾选，则匀速缩放）（如果原动画没有位移，即便勾选了也是匀速）*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (EditCondition = "MotionWarpingType != EWKRootMotionWarpingType::WarpingWithRotation",EditConditionHides))
	bool bKeepAnimationRhythm = true;
	
	/** 位移缩放 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MotionWarpingType == EWKRootMotionWarpingType::WarpingWithTranslation",EditConditionHides))
	FWKRootMotionWarping_Translation WarpingTranslation;

	/** 重载旋转 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MotionWarpingType == EWKRootMotionWarpingType::WarpingWithRotation", EditConditionHides))
	FWKRootMotionWarping_Rotation WarpingRotation;  

	/** 目标追踪*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MotionWarpingType == EWKRootMotionWarpingType::WarpingWithTarget", EditConditionHides))
	FWKRootMotionWarping_TargetActor WarpingTargetActor;
	
	/** 给定位置*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MotionWarpingType == EWKRootMotionWarpingType::WarpingWithLocation", EditConditionHides))
	FDSRootMotionWarping_Location WarpingLocation;
};
