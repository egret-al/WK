// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAttributeSetBase.h"

#include "GameplayEffectExtension.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/Player/WKPlayerController.h"
#include "Net/UnrealNetwork.h"

UWKAttributeSetBase::UWKAttributeSetBase()
{
}

UWKAbilitySystemComponent* UWKAttributeSetBase::GetWKAbilitySystemComponent() const
{
	return Cast<UWKAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
