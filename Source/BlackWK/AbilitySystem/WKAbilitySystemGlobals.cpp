// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilitySystemGlobals.h"

#include "WKGameplayEffectContext.h"

UWKAbilitySystemGlobals::UWKAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UWKAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FWKGameplayEffectContext();
}
