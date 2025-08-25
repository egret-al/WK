// Fill out your copyright notice in the Description page of Project Settings.


#include "WKTargetData.h"

bool FGameplayAbilityTargetData_Integer::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << IntegerData;
	bOutSuccess = true;
	return true;
}
