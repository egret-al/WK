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
	
	// Set attributes
	UIHUDWidget->SetCurrentHealth(PS->GetHealth());
	UIHUDWidget->SetMaxHealth(PS->GetMaxHealth());
	UIHUDWidget->SetHealthPercentage(PS->GetHealth() / FMath::Max<float>(PS->GetMaxHealth(), 1.f));
	UIHUDWidget->SetCurrentMana(PS->GetMana());
	UIHUDWidget->SetMaxMana(PS->GetMaxMana());
	UIHUDWidget->SetManaPercentage(PS->GetMana() / FMath::Max<float>(PS->GetMaxMana(), 1.f));
	UIHUDWidget->SetHealthRegenRate(PS->GetHealthRegenRate());
	UIHUDWidget->SetManaRegenRate(PS->GetManaRegenRate());
	UIHUDWidget->SetCurrentStamina(PS->GetStamina());
	UIHUDWidget->SetMaxStamina(PS->GetMaxStamina());
	UIHUDWidget->SetStaminaPercentage(PS->GetStamina() / FMath::Max<float>(PS->GetMaxStamina(), 1.f));
	UIHUDWidget->SetStaminaRegenRate(PS->GetStaminaRegenRate());
	UIHUDWidget->SetCurrentHulu(PS->GetHulu());
	UIHUDWidget->SetMaxHulu(PS->GetMaxHulu());
	UIHUDWidget->SetHuluPercentage(PS->GetHulu() / FMath::Max<float>(PS->GetMaxHulu(), 1.f));
	UIHUDWidget->SetHuluRegenRate(PS->GetHuluRegenRate());
	UIHUDWidget->SetExperience(PS->GetXP());
	UIHUDWidget->SetGold(PS->GetGold());
	UIHUDWidget->SetHeroLevel(PS->GetCharacterLevel());
}

UWKWidgetHUD* AWKPlayerController::GetHUD()
{
	return UIHUDWidget;
}

void AWKPlayerController::ShowDamageNumber_Implementation(float DamageAmount, AWKCharacterBase* TargetCharacter)
{
	if (TargetCharacter && DamageNumberClass)
	{
		UWKDamageTextWidgetComponent* DamageText = NewObject<UWKDamageTextWidgetComponent>(TargetCharacter, DamageNumberClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->SetDamageText(DamageAmount);
	}
}

bool AWKPlayerController::ShowDamageNumber_Validate(float DamageAmount, AWKCharacterBase* TargetCharacter)
{
	return true;
}

void AWKPlayerController::SetRespawnCountdown_Implementation(float RespawnTimeRemaining)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetRespawnCountdown(RespawnTimeRemaining);
	}
}

bool AWKPlayerController::SetRespawnCountdown_Validate(float RespawnTimeRemaining)
{
	return true;
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
	if (AWKCharacterBase* CharacterBase = Cast<AWKCharacterBase>(GetPawn()))
	{
		if (UWKAbilitySystemComponent* ASC = CharacterBase->GetWKAbilitySystemComponent())
		{
			ASC->ProcessAbilityInput();
		}
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}
