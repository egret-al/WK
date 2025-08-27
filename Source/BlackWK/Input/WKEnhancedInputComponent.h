// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "WKInputConfig.h"
#include "WKEnhancedInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLACKWK_API UWKEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UWKEnhancedInputComponent(const FObjectInitializer& ObjectInitializer);

	void AddInputMappings(const UWKInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UWKInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveBinds(TArray<uint32>& BindHandles);

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UWKInputConfig* InputConfig, const FName InputName, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UWKInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);
};

template <class UserClass, typename FuncType>
void UWKEnhancedInputComponent::BindNativeAction(const UWKInputConfig* InputConfig, const FName InputName, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);

	if (const UInputAction* IA = InputConfig->FindNativeInputActionForName(InputName, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UWKEnhancedInputComponent::BindAbilityActions(const UWKInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FWKInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputName.IsValid())
		{
			if (PressedFunc)
			{
				if (Action.bIsTrigger)
				{
					BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputName, Action.bCheckLongPress).GetHandle());
				}
				else
				{
					BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputName, Action.bCheckLongPress).GetHandle());
				}
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputName).GetHandle());
			}
		}
	}
}