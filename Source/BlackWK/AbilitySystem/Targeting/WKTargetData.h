// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "BlackWK/Animation/AnimNotify/WKAnimNotifyState_RootMotionWarping.h"
#include "UObject/Object.h"
#include "WKTargetData.generated.h"

/** Target data with just a integer */
USTRUCT(BlueprintType)
struct  FGameplayAbilityTargetData_Integer : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	int32 IntegerData;

	FGameplayAbilityTargetData_Integer()
		: IntegerData(ForceInit)
	{};

	FGameplayAbilityTargetData_Integer(int32 InInteger)
		: IntegerData(InInteger)
	{};

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual FString ToString() const override
	{
		return TEXT("FGameplayAbilityTargetData_Integer");
	}

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Integer> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Integer>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};

USTRUCT(BlueprintType)
struct FGameplayAbilityTargetData_PlayMontage : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

public:
	FGameplayAbilityTargetData_PlayMontage()
		: ActorRotator(ForceInit)
		, ControlRotator(ForceInit)
		, MoveInPut(ForceInit)
		, TargetActor(nullptr)
	{}

	FGameplayAbilityTargetData_PlayMontage(const FRotator& InActorRotator, const FRotator& InControlRotator, const FVector& InMoveInPut, AActor* InTargetActor)
		: ActorRotator(InActorRotator)
		, ControlRotator(InControlRotator)
		, MoveInPut(InMoveInPut)
		, TargetActor(InTargetActor)
	{}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual FString ToString() const override
	{
		return TEXT("FGameplayAbilityTargetData_PlayMontage");
	}

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	FRotator ActorRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	FRotator ControlRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	FVector_NetQuantize MoveInPut;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	TWeakObjectPtr<AActor> TargetActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	TArray<float> OverrideWarpingDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	TArray<FVector_NetQuantize100> OverrideWarpingDirection;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	TArray<FVector_NetQuantize100> CustomTargetLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	TArray<float> OverrideWarpingRotation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	FWKOverrideWarpingTargetActorParam OverrideWarpingTargetActorParam;
};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_PlayMontage> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_PlayMontage>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};