// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayEffectContext.h"

#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "WKAbilitySourceInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WKGameplayEffectContext)

class FArchive;

FWKGameplayEffectContext* FWKGameplayEffectContext::ExtractEffectContext(FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FWKGameplayEffectContext::StaticStruct()))
	{
		return static_cast<FWKGameplayEffectContext*>(BaseEffectContext);
	}

	return nullptr;
}

bool FWKGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

void FWKGameplayEffectContext::SetAbilitySource(const IWKAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IWKAbilitySourceInterface* FWKGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IWKAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FWKGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}