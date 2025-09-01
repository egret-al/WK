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

void AWKPlayerController::SetRotationYawLimitBaseNow(float LimitYawInDegrees, FRotator InLimitRotationBase)
{
	bIsLimitRotationYaw = true;
	if (InLimitRotationBase == FRotator::ZeroRotator)
	{
		LimitRotationBase = GetControlRotation();
	}
	else
	{
		LimitRotationBase = InLimitRotationBase;
	}
	LimitYaw = LimitYawInDegrees;
}

void AWKPlayerController::SetRotationPitchLimitBaseNow(float LimitPitchInDegrees, FRotator InLimitRotationBase)
{
	bIsLimitRotationPitch = true;
	if (InLimitRotationBase == FRotator::ZeroRotator)
	{
		LimitRotationBase = GetControlRotation();
	}
	else
	{
		LimitRotationBase = InLimitRotationBase;
	}
	LimitPitch = LimitPitchInDegrees;
}

void AWKPlayerController::ClearRotationLimit()
{
	bIsLimitRotationYaw = false;
	bIsLimitRotationPitch = false;
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

void AWKPlayerController::UpdateRotation(float DeltaTime)
{
	if (bLockControlRotation)
	{
		RotationInput = FRotator::ZeroRotator;
	}
	//限制转向速度
	RotationInput *= TurnRate;

	if (bIsLimitRotationYaw)
	{
		const FRotator DeltaRot(RotationInput);
		const FRotator OldRot = GetControlRotation();
		const float NewYaw = OldRot.Yaw + DeltaRot.Yaw;
		const float DeltaAngle = FMath::FindDeltaAngleDegrees(NewYaw, LimitRotationBase.Yaw);
		//超过限制忽略输入
		if (DeltaAngle < (-1 * LimitYaw / 2) || DeltaAngle>(LimitYaw / 2))
		{
			RotationInput.Yaw = 0.f;
		}
	}
	if (bIsLimitRotationPitch)
	{
		const FRotator DeltaRot(RotationInput);
		const FRotator OldRot = GetControlRotation();
		const float NewPitch = OldRot.Pitch + DeltaRot.Pitch;
		const float DeltaAngle = FMath::FindDeltaAngleDegrees(NewPitch, LimitRotationBase.Pitch);
		// 超过限制忽略输入
		if (DeltaAngle < -1 * LimitPitch / 2 || DeltaAngle> LimitPitch / 2)
		{
			RotationInput.Pitch = 0.f;
		}
	}
	
	bHasRotationInputThisFrame = !RotationInput.IsNearlyZero(0.001f);
	
	Super::UpdateRotation(DeltaTime);
}

void AWKPlayerController::SetTurnRate(float NewRate)
{
	TurnRate = FMath::Clamp(NewRate, 0.f, 100.f);
}

void AWKPlayerController::ResetTurnRate()
{
	TurnRate = 1.f;
}
