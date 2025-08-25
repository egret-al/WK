// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
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
		return FGameplayAbilityTargetData_Integer::StaticStruct();
	}

	virtual FString ToString() const override
	{
		return TEXT("FGameplayAbilityTargetData_Integer");
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Integer> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Integer>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};