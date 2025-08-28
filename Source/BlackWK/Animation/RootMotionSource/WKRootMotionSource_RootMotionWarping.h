// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/RootMotionSource.h"
#include "UObject/Object.h"
#include "WKRootMotionSource_RootMotionWarping.generated.h"

enum class EWKRotateOrientation : uint8;

USTRUCT()
struct FRootMotionWarpingSection
{
	GENERATED_USTRUCT_BODY();

public:
	FRootMotionWarpingSection();
	virtual ~FRootMotionWarpingSection() = default;
	FRootMotionWarpingSection(float InStartTime,float InEndTime);
	FRootMotionWarpingSection(float InStartTime,float InEndTime, ERootMotionAccumulateMode InAccumulateMode,bool InbIgnoreZAccumulate,bool InbMaintainVelocityWhenFinish,bool InbKeepAnimationRhythm);
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
	virtual UStruct* GetStruct() const { return FRootMotionWarpingSection::StaticStruct(); }
	
	UPROPERTY()
	float StartTime;

	UPROPERTY()
	float EndTime;

	UPROPERTY()
	ERootMotionAccumulateMode AccumulateMode;

	UPROPERTY()
	bool bIgnoreZAccumulate;

	UPROPERTY()
	bool bMaintainVelocityWhenFinish;

	UPROPERTY()
	bool bKeepAnimationRhythm;
};

USTRUCT()
struct FRootMotionWarpingSection_Translation : public FRootMotionWarpingSection
{
	GENERATED_USTRUCT_BODY();

public:
	FRootMotionWarpingSection_Translation();
	virtual ~FRootMotionWarpingSection_Translation() override = default;
	FRootMotionWarpingSection_Translation(float InStartTime,float InEndTime,ERootMotionAccumulateMode InAccumulateMode,bool InbIgnoreZAccumulate,bool InbMaintainVelocityWhenFinish,bool InbKeepAnimationRhythm);
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	virtual UStruct* GetStruct() const override { return StaticStruct(); }

	/** 距离*/
	UPROPERTY()
	float WarpingDistance;

	/** 位移方向*/
	UPROPERTY()
	FVector_NetQuantize10 Direction;

	TOptional<float> AnimationTotalDistance;
};

USTRUCT()
struct FRootMotionWarpingSection_Rotation : public FRootMotionWarpingSection
{
	GENERATED_USTRUCT_BODY();

public:
	FRootMotionWarpingSection_Rotation();
	FRootMotionWarpingSection_Rotation(float InStartTime,float InEndTime);
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	virtual UStruct* GetStruct() const override { return StaticStruct(); }
	
	/** 旋转角度 只有EDSRootMotionRotationType不等于TurnToTarget时生效）*/
	UPROPERTY()
	float WarpingRotation;

	/** 旋转目标（只有EDSRootMotionRotationType::TurnToTarget时生效）*/
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> TargetActor;

	/** 旋转角速度（只有EDSRootMotionRotationType::TurnToTarget时生效）*/
	UPROPERTY()
	float RotationAngleSpeed;

	/** 旋转方向（只有EDSRootMotionRotationType::TurnToTarget时生效）*/
	UPROPERTY()
	EWKRotateOrientation RotationOrientation;
	
	TOptional<float> StartRotation;
};

USTRUCT()
struct FRootMotionWarpingSection_TargetActor : public FRootMotionWarpingSection
{
	GENERATED_USTRUCT_BODY();

public:
	FRootMotionWarpingSection_TargetActor();
	virtual ~FRootMotionWarpingSection_TargetActor() override = default;
	FRootMotionWarpingSection_TargetActor(float InStartTime,float InEndTime,ERootMotionAccumulateMode InAccumulateMode,bool InbIgnoreZAccumulate,bool InbMaintainVelocityWhenFinish,bool InbKeepAnimationRhythm);
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	virtual UStruct* GetStruct() const override { return StaticStruct(); }
	
	/** 目标*/
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> TargetActor;

	/** 是否在过程中更新目标坐标 */
	UPROPERTY()
	bool bUpdateTargetPosition;

	/** 最大追踪距离 */
	UPROPERTY()
	float MaxDistance;

	UPROPERTY()
	float WarpingDistanceIfTargetNotFound;

	/** 目标点追踪速度(不是自身速度) */
	UPROPERTY()
	float ChasingSpeed;

	/** 放弃追踪角度 */
	UPROPERTY()
	float RotationToAbort;

	/** 旋转速率 */
	UPROPERTY()
	float RotationRate;

	/** 最大旋转角度 */
	UPROPERTY()
	float RotationMax;

	/** 最终坐标偏移 */
	UPROPERTY()
	FVector_NetQuantize10 LocationOffset;

	/** 最终旋转偏移 */
	UPROPERTY()
	float RotationOffset;

	UPROPERTY()
	TObjectPtr<UCurveFloat> ZTrajectory;

	/** 旋转方向（只有EDSRootMotionRotationType::TurnToTarget时生效）*/
	UPROPERTY()
	EWKRotateOrientation RotationOrientation;
	
	UPROPERTY()
	FVector LastTargetLocation;

	UPROPERTY()
	FVector LastWarpingLocation;
	
	UPROPERTY()
	FVector LastDirection;
	
	TOptional<FVector> StartLocation;
	TOptional<float> AnimationTotalDistance;
	float StartDeltaZ;
	bool StopTrace;
	float CurrentRotatorAngle;
};

USTRUCT()
struct FRootMotionWarpingSection_Location : public FRootMotionWarpingSection
{
	GENERATED_USTRUCT_BODY();

public:
	FRootMotionWarpingSection_Location();
	virtual ~FRootMotionWarpingSection_Location() override = default;
	FRootMotionWarpingSection_Location(float InStartTime,float InEndTime,ERootMotionAccumulateMode InAccumulateMode,bool InbIgnoreZAccumulate,bool InbMaintainVelocityWhenFinish,bool InbKeepAnimationRhythm);
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	virtual UStruct* GetStruct() const override{ return StaticStruct(); }
	
	UPROPERTY()
	FVector_NetQuantize10 TargetLocation;

	UPROPERTY()
	TObjectPtr<UCurveFloat> ZTrajectory;
	
	float AnimationTotalDistance;
	TOptional<FVector> StartLocation;
	float StartDeltaZ;
};

template<>
struct TStructOpsTypeTraits<FRootMotionWarpingSection> : public TStructOpsTypeTraitsBase2<FRootMotionWarpingSection>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

/**
 * 
 */
USTRUCT()
struct BLACKWK_API FWKRootMotionSource_RootMotionWarping : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()

public:
	FWKRootMotionSource_RootMotionWarping();
	virtual ~FWKRootMotionSource_RootMotionWarping() override;

	virtual FRootMotionSource* Clone() const override;
	virtual bool Matches(const FRootMotionSource* Other) const override;
	virtual void PrepareRootMotion(float SimulationTime, float MovementTickTime, const ACharacter& Character, const UCharacterMovementComponent& MoveComponent) override;
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual FString ToSimpleString() const override;
	
	static FName RootMotionWarpingForceName;

	UPROPERTY()
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY()
	float AnimationStartPos;

	UPROPERTY()
	float AnimationEndPos;
	
	TSharedPtr<FRootMotionWarpingSection> LastSection;
	
	UPROPERTY()
	TArray<FRootMotionWarpingSection_Translation> WarpingSections_Translation;

	UPROPERTY()
	TArray<FRootMotionWarpingSection_Rotation> WarpingSections_Rotation;

	UPROPERTY()
	TArray<FRootMotionWarpingSection_TargetActor> WarpingSections_TargetActor;
	
	UPROPERTY()
	TArray<FRootMotionWarpingSection_Location> WarpingSections_Location;

private:
	bool HandleTargetActorWarping(const ACharacter& Character, const float& SimulationTime, FTransform& DeltaTransform, FRotator& DeltaRotation, FRootMotionWarpingSection*& ActiveSection);
	bool HandleLocationWarping(const ACharacter& Character, const float& SimulationTime, FTransform& DeltaTransform, FRotator& DeltaRotation, FRootMotionWarpingSection*& ActiveSection);
	bool HandleTranslationWarping(const ACharacter& Character, const float& SimulationTime, FTransform& DeltaTransform, FRotator& DeltaRotation, FRootMotionWarpingSection*& ActiveSection);
	bool HandleRotationWarping(const ACharacter& Character, const float& SimulationTime, FRotator& DeltaRotation);
	void HandleRootMotionSourceParams(FRootMotionWarpingSection* ActiveSection, const UCharacterMovementComponent& MoveComponent);
	float CalculateDeltaAngle(float CurrentYaw,float DesiredYaw,float RotationAngleSpeed, float DeltaSeconds, EWKRotateOrientation RotationOrientation);
	
private:
	float PlayRate;
	float CurrentAnimPos;
};

template<>
struct TStructOpsTypeTraits<FWKRootMotionSource_RootMotionWarping> : public TStructOpsTypeTraitsBase2<FWKRootMotionSource_RootMotionWarping>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};