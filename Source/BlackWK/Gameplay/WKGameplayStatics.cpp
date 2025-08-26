// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayStatics.h"

#include "AbilitySystemInterface.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"

bool UWKGameplayStatics::WorldIsGame(const UObject* WorldContextObj)
{
	if (WorldContextObj && WorldContextObj->GetWorld())
	{
		const EWorldType::Type WorldType = WorldContextObj->GetWorld()->WorldType;
		if (WorldType == EWorldType::PIE || WorldType == EWorldType::Game)
		{
			return true;
		}
	}

	return false;
}

UWKAbilitySystemComponent* UWKGameplayStatics::GetWKAbilitySystemComponent(AActor* InActor)
{
	if (!InActor)
	{
		return nullptr;
	}

	if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(InActor))
	{
		return Cast<UWKAbilitySystemComponent>(AbilityInterface->GetAbilitySystemComponent());
	}
	
	return nullptr;
}
