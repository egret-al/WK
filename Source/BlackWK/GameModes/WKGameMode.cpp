// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameMode.h"

#include "WKGameState.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/Player/WKPlayerController.h"
#include "BlackWK/Player/WKPlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"

AWKGameMode::AWKGameMode()
{
	RespawnDelay = 5.0f;
	DefaultPawnClass = AWKCharacterBase::StaticClass();
	PlayerControllerClass = AWKPlayerController::StaticClass();
	PlayerStateClass = AWKPlayerState::StaticClass();
	GameStateClass = AWKGameState::StaticClass();
}

void AWKGameMode::HeroDied(AController* Controller)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, Controller->GetPawn()->GetActorTransform(), SpawnParameters);

	Controller->UnPossess();
	Controller->Possess(SpectatorPawn);

	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::RespawnHero, Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}

void AWKGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Get the enemy hero spawn point
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		if (Actor->GetName() == FString("EnemyHeroSpawn"))
		{
			EnemySpawnPoint = Actor;
			break;
		}
	}
}

void AWKGameMode::RespawnHero(AController* Controller)
{
	if (Controller->IsPlayerController())
	{
		// Respawn player hero
		AActor* PlayerStart = FindPlayerStart(Controller);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AWKCharacterBase* Hero = GetWorld()->SpawnActor<AWKCharacterBase>(HeroClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParameters);

		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Hero);
	}
	else
	{
		// Respawn AI hero
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AWKCharacterBase* Hero = GetWorld()->SpawnActor<AWKCharacterBase>(HeroClass, EnemySpawnPoint->GetActorTransform(), SpawnParameters);
		
		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Hero);
	}
}
