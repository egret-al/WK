// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "WKAbilityTypes.generated.h"

#define ACTOR_ROLE_FSTRING *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(GetLocalRole()))
#define GET_ACTOR_ROLE_FSTRING(Actor) *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(Actor->GetLocalRole()))

UENUM(BlueprintType)
enum class EWKAbilityInputID : uint8
{
	// 0 None
	None			UMETA(DisplayName = "None"),
	// 1 Confirm
	Confirm			UMETA(DisplayName = "Confirm"),
	// 2 Cancel
	Cancel			UMETA(DisplayName = "Cancel"),
	// 3 LMB
	Ability1		UMETA(DisplayName = "Ability1"),
	// 4 RMB
	Ability2		UMETA(DisplayName = "Ability2"),
	// 5 Q
	Ability3		UMETA(DisplayName = "Ability3"),
	// 6 E
	Ability4		UMETA(DisplayName = "Ability4"),
	// 7 R
	Ability5		UMETA(DisplayName = "Ability5"),
	// 8 Sprint
	Sprint			UMETA(DisplayName = "Sprint"),
	// 9 Jump
	Jump			UMETA(DisplayName = "Jump")
};

UENUM(BlueprintType)
enum class EWKGameplayAbilityInputBinds : uint8
{
	None,
	MeleeCombo		UMETA(DisplayName = "MeleeCombo")
};

/**
 * GUID 和 AbilityHandle 组成的用于 TMap 的 Key。
 * 现用于 Client 与 Server 的数据同步
 */
USTRUCT()
struct FGameplayAbilityGuidKey
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityGuidKey()
	{}

	FGameplayAbilityGuidKey(const FGameplayAbilitySpecHandle& HandleRef, const FGuid& InGuid)
		: AbilityHandle(HandleRef), Guid(InGuid)
	{}

	bool operator==(const FGameplayAbilityGuidKey& Other) const
	{
		return AbilityHandle == Other.AbilityHandle && Guid == Other.Guid;
	}

	bool operator!=(const FGameplayAbilityGuidKey& Other) const
	{
		return AbilityHandle != Other.AbilityHandle || Guid != Other.Guid;
	}

	friend uint32 GetTypeHash(const FGameplayAbilityGuidKey& Handle)
	{
		return GetTypeHash(Handle.AbilityHandle) ^ GetTypeHash(Handle.Guid);
	}

	UPROPERTY()
	FGameplayAbilitySpecHandle AbilityHandle;

	UPROPERTY()
	FGuid Guid;
};

struct FWKAbilityReplicatedDataCache : public FAbilityReplicatedDataCache
{
	FWKAbilityReplicatedDataCache() : DelegateCount(0) {}

	// 记录Delegate添加的次数
	int32 DelegateCount;
};