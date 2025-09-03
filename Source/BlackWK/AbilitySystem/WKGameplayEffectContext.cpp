// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayEffectContext.h"

#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

bool FWKGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsLoading())
	{
		TargetData.Clear();
	}

	TargetData.NetSerialize(Ar, Map, bOutSuccess);
	Ar << SkillID;
	return Super::NetSerialize(Ar, Map, bOutSuccess);
}

FWKGameplayEffectContext* FWKGameplayEffectContext::Duplicate() const
{
	FWKGameplayEffectContext* NewContext = new FWKGameplayEffectContext();
	*NewContext = *this;
	NewContext->AddActors(Actors);
	if (GetHitResult())
	{
		NewContext->AddHitResult(*GetHitResult(), true);
	}
	NewContext->TargetData.Append(TargetData);
	NewContext->SkillID = SkillID;
	return NewContext;
}

