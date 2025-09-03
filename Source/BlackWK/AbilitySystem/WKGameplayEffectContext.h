// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "WKGameplayEffectContext.generated.h"

class IWKAbilitySourceInterface;

/**
 * 
 */
USTRUCT()
struct FWKGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	virtual FWKGameplayEffectContext* Duplicate() const override;
	virtual UScriptStruct* GetScriptStruct() const override { return FWKGameplayEffectContext::StaticStruct(); }
	
	virtual FGameplayAbilityTargetDataHandle GetTargetData() { return TargetData; }
	virtual void AddTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle) { TargetData.Append(TargetDataHandle); }
	virtual int32 GetSkillID() const { return SkillID; }
	virtual void SetSkillID(int32 InSkillID) { SkillID = InSkillID; }

protected:
	UPROPERTY(Transient)
	FGameplayAbilityTargetDataHandle TargetData;

	// 技能ID
	int32 SkillID = INDEX_NONE;
};

template<>
struct TStructOpsTypeTraits<FWKGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FWKGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};