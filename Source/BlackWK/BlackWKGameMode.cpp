// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlackWKGameMode.h"
#include "BlackWKCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABlackWKGameMode::ABlackWKGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
