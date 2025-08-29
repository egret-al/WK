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
