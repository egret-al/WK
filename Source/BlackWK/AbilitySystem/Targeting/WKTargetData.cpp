// Fill out your copyright notice in the Description page of Project Settings.


#include "WKTargetData.h"

bool FGameplayAbilityTargetData_Integer::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << IntegerData;
	bOutSuccess = true;
	return true;
}

bool FGameplayAbilityTargetData_PlayMontage::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << ActorRotator;
	Ar << ControlRotator;
	MoveInPut.NetSerialize(Ar, Map, bOutSuccess);
	Ar << TargetActor;
	SafeNetSerializeTArray_Default<8>(Ar, OverrideWarpingDistance);
	SafeNetSerializeTArray_WithNetSerialize<8>(Ar, OverrideWarpingDirection, Map);
	SafeNetSerializeTArray_WithNetSerialize<8>(Ar, CustomTargetLocation,Map);
	SafeNetSerializeTArray_Default<8>(Ar, OverrideWarpingRotation);

	bOutSuccess = true;
	return true;
}
