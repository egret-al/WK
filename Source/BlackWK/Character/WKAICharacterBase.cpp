// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAICharacterBase.h"

#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/AttributeSets/WKAttributeSetBase.h"
#include "BlackWK/AI/WKAIStateInterface.h"
#include "BlackWK/UI/WKFloatingStatusBarWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

AWKAICharacterBase::AWKAICharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create ability system component, and set it to be explicitly replicated
	HardRefAbilitySystemComponent = CreateDefaultSubobject<UWKAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	HardRefAbilitySystemComponent->SetIsReplicated(true);

	// Minimal Mode means that no GameplayEffects will replicate. They will only live on the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	HardRefAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Set our parent's TWeakObjectPtr
	AbilitySystemComponent = HardRefAbilitySystemComponent;

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	HardRefAttributeSetBase = CreateDefaultSubobject<UWKAttributeSetBase>(TEXT("AttributeSetBase"));

	// Set our parent's TWeakObjectPtr
	AttributeSetBase = HardRefAttributeSetBase;

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

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->InitAbilityActorInfo(GetPlayerState(), this);

		// Setup FloatingStatusBar UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
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
					UIFloatingStatusBar->SetHealthPercentage(GetHealth() / GetMaxHealth());

					UIFloatingStatusBar->SetCharacterName(CharacterName);
				}
			}
		}

		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);

		// Tag change callbacks
		// AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::StunTagChanged);
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

void AWKAICharacterBase::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	// Update floating status bar
	if (UIFloatingStatusBar)
	{
		UIFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
	}

	// If the minion died, handle death
	// if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	// {
	// 	Die();
	// }
}

// void AWKAICharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
// {
// 	if (NewCount > 0)
// 	{
// 		FGameplayTagContainer AbilityTagsToCancel;
// 		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));
//
// 		FGameplayTagContainer AbilityTagsToIgnore;
// 		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));
//
// 		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
// 	}
// }
