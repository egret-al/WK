// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimInstanceBase.h"

#include "BlackWK/Character/WKCharacterBase.h"

void UWKAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AWKCharacterBase* OwnerCharacter = Cast<AWKCharacterBase>(TryGetPawnOwner()))
	{
		OwnerWK = OwnerCharacter;
	}
}
