// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WKGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API AWKGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWKGameMode();
	void HeroDied(AController* Controller);

protected:
	virtual void BeginPlay() override;
	void RespawnHero(AController* Controller);
	
protected:
	float RespawnDelay;
	TSubclassOf<class AWKCharacterBase> HeroClass;
	AActor* EnemySpawnPoint;
};
