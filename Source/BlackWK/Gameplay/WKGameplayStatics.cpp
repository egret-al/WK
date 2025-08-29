// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayStatics.h"

#include "AbilitySystemInterface.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/Player/WKPlayerController.h"

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

APlayerController* UWKGameplayStatics::GetLocalPlayerController(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (PlayerController && PlayerController->GetLocalPlayer() && PlayerController->GetLocalPlayer()->IsA<ULocalPlayer>())
			{
				return PlayerController;
			}
		}
	}
	return nullptr;
}

TArray<AWKPlayerController*> UWKGameplayStatics::GetAllPlayerController(const UObject* WorldContextObject)
{
	TArray<AWKPlayerController*> Out;

	if (WorldContextObject->GetWorld())
	{
		for (FConstPlayerControllerIterator Iterator = WorldContextObject->GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (AWKPlayerController* PC = Cast<AWKPlayerController>(Iterator->Get()))
			{
				Out.Add(PC);
			}
		}
	}
	return Out;
}
