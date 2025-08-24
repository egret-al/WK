// Fill out your copyright notice in the Description page of Project Settings.


#include "WKInputConfig.h"

UWKInputConfig::UWKInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

const UInputAction* UWKInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FWKInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}

const UInputAction* UWKInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FWKInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}