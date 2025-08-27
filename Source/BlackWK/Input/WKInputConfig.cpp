// Fill out your copyright notice in the Description page of Project Settings.


#include "WKInputConfig.h"

UWKInputConfig::UWKInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

const UInputAction* UWKInputConfig::FindNativeInputActionForName(const FName InputName, bool bLogNotFound) const
{
	for (const FWKInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && Action.InputName == InputName)
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}

const UInputAction* UWKInputConfig::FindAbilityInputActionForName(const FName InputName, bool bLogNotFound) const
{
	for (const FWKInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputName == InputName)
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}