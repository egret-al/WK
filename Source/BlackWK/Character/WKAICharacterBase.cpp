// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAICharacterBase.h"

#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AI/WKAIStateInterface.h"
#include "BlackWK/Player/WKPlayerState.h"
#include "BlackWK/UI/WKFloatingStatusBarWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

AWKAICharacterBase::AWKAICharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
	UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));

	UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/_Game/UI/Widget/WBP_EnemyFloatingStatusBar.WBP_EnemyFloatingStatusBar_C"));
	if (!UIFloatingStatusBarClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
	}
}

void AWKAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	AWKPlayerState* PS = Cast<AWKPlayerState>(GetPlayerState());
	check(PS);
	AbilitySystemComponent = PS->GetWKAbilitySystemComponent();

	AbilitySystemComponent->InitAbilityActorInfo(GetPlayerState(), this);
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->IsLocalPlayerController())
	{
		if (UIFloatingStatusBarClass)
		{
			UIFloatingStatusBar = CreateWidget<UWKFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
			if (UIFloatingStatusBar && UIFloatingStatusBarComponent)
			{
				UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

				// Setup the floating status bar
				UIFloatingStatusBar->SetCharacterName(CharacterName);
			}
		}
	}
}

bool AWKAICharacterBase::HasSawEnemyTarget()
{
	if (IWKAIStateInterface* StateController = Cast<IWKAIStateInterface>(GetController()))
	{
		return StateController->HasSawEnemyTarget();
	}
	return false;
}

FVector AWKAICharacterBase::GetLockLocation_Implementation()
{
	return GetActorLocation();
}
