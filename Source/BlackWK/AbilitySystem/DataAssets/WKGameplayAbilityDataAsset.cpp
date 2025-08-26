// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayAbilityDataAsset.h"

FWKGameplayAbilityPriorityInfo::FWKGameplayAbilityPriorityInfo()
    : Priority(INDEX_NONE)
    , bCanMove(true)
    , bCheckPriorityActivateOnly(false)
    , bCancelLastActiveAbility(false)
    , bForceActivateAbility(false)
{
}
