// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAIDaShengCharacter.h"

#include "AbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/WKGameplayTags.h"

void AWKAIDaShengCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->AddLooseGameplayTag(WKGameplayTags::Gameplay_State_Invincible);
	}
}

AWKCharacterBase* AWKAIDaShengCharacter::GetCurrentTarget()
{
	if (IWKAIStateInterface* ControllerInterface = Cast<IWKAIStateInterface>(GetController()))
	{
		return ControllerInterface->GetCurrentTarget();
	}
	return nullptr;
}
