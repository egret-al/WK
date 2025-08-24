// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WKPlayerController.generated.h"

class UWKDamageTextWidgetComponent;
class UWKWidgetHUD;
class AWKCharacterBase;

UCLASS()
class BLACKWK_API AWKPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void CreateHUD();
	UWKWidgetHUD* GetHUD();

	UFUNCTION(Client, Reliable, WithValidation)
	void ShowDamageNumber(float DamageAmount, AWKCharacterBase* TargetCharacter);
	void ShowDamageNumber_Implementation(float DamageAmount, AWKCharacterBase* TargetCharacter);
	bool ShowDamageNumber_Validate(float DamageAmount, AWKCharacterBase* TargetCharacter);

	// Simple way to RPC to the client the countdown until they respawn from the GameMode. Will be latency amount of out sync with the Server.
	UFUNCTION(Client, Reliable, WithValidation)
	void SetRespawnCountdown(float RespawnTimeRemaining);
	void SetRespawnCountdown_Implementation(float RespawnTimeRemaining);
	bool SetRespawnCountdown_Validate(float RespawnTimeRemaining);

protected:
	// Server only
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "WKConfig|UI")
	TSubclassOf<UWKDamageTextWidgetComponent> DamageNumberClass;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WKConfig|UI")
	TSubclassOf<UWKWidgetHUD> UIHUDWidgetClass;
	
	UPROPERTY(BlueprintReadWrite, Category = "WKConfig|UI")
	UWKWidgetHUD* UIHUDWidget;
};
