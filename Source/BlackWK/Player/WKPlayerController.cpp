// Fill out your copyright notice in the Description page of Project Settings.


#include "WKPlayerController.h"

#include "AbilitySystemComponent.h"
#include "WKPlayerState.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/UI/WKDamageTextWidgetComponent.h"
#include "BlackWK/UI/WKWidgetHUD.h"

void AWKPlayerController::CreateHUD()
{
	// Only create once
	if (UIHUDWidget)
	{
		return;
	}

	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController())
	{
		return;
	}

	// Need a valid PlayerState to get attributes from
	AWKPlayerState* PS = GetPlayerState<AWKPlayerState>();
	if (!PS)
	{
		return;
	}

	UIHUDWidget = CreateWidget<UWKWidgetHUD>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();
}

UWKWidgetHUD* AWKPlayerController::GetHUD()
{
	return UIHUDWidget;
}

void AWKPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AWKPlayerState* PS = GetPlayerState<AWKPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void AWKPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	CreateHUD();
}

void AWKPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	// if (AWKCharacterBase* CharacterBase = Cast<AWKCharacterBase>(GetPawn()))
	// {
	// 	if (UWKAbilitySystemComponent* ASC = CharacterBase->GetWKAbilitySystemComponent())
	// 	{
	// 		ASC->ProcessAbilityInput();
	// 	}
	// }
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}
