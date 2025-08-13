// Fill out your copyright notice in the Description page of Project Settings.


#include "WKPlayerState.h"

#include "WKPlayerController.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/AttributeSets/WKAttributeSetBase.h"
#include "BlackWK/Character/WKPlayerCharacterBase.h"
#include "BlackWK/UI/WKFloatingStatusBarWidget.h"
#include "BlackWK/UI/WKWidgetHUD.h"

AWKPlayerState::AWKPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UWKAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UWKAttributeSetBase>(TEXT("AttributeSetBase"));

	NetUpdateFrequency = 100.0f;
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void AWKPlayerState::BeginPlay()
{
	Super::BeginPlay();

		if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AWKPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &AWKPlayerState::MaxHealthChanged);
		HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthRegenRateAttribute()).AddUObject(this, &AWKPlayerState::HealthRegenRateChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &AWKPlayerState::ManaChanged);
		MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxManaAttribute()).AddUObject(this, &AWKPlayerState::MaxManaChanged);
		ManaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaRegenRateAttribute()).AddUObject(this, &AWKPlayerState::ManaRegenRateChanged);
		StaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetStaminaAttribute()).AddUObject(this, &AWKPlayerState::StaminaChanged);
		MaxStaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxStaminaAttribute()).AddUObject(this, &AWKPlayerState::MaxStaminaChanged);
		StaminaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetStaminaRegenRateAttribute()).AddUObject(this, &AWKPlayerState::StaminaRegenRateChanged);
		HuluChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHuluAttribute()).AddUObject(this, &AWKPlayerState::HuluChanged);
		MaxHuluChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHuluAttribute()).AddUObject(this, &AWKPlayerState::MaxHuluChanged);
		HuluRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHuluRegenRateAttribute()).AddUObject(this, &AWKPlayerState::HuluRegenRateChanged);
		XPChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetXPAttribute()).AddUObject(this, &AWKPlayerState::XPChanged);
		GoldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetGoldAttribute()).AddUObject(this, &AWKPlayerState::GoldChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetCharacterLevelAttribute()).AddUObject(this, &AWKPlayerState::CharacterLevelChanged);

		// Tag change callbacks
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AWKPlayerState::StunTagChanged);
	}
}

UAbilitySystemComponent* AWKPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UWKAttributeSetBase* AWKPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

bool AWKPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void AWKPlayerState::ShowAbilityConfirmCancelText(bool ShowText)
{
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->ShowAbilityConfirmCancelText(ShowText);
		}
	}
}

float AWKPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float AWKPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AWKPlayerState::GetHealthRegenRate() const
{
	return AttributeSetBase->GetHealthRegenRate();
}

float AWKPlayerState::GetMana() const
{
	return AttributeSetBase->GetMana();
}

float AWKPlayerState::GetMaxMana() const
{
	return AttributeSetBase->GetMaxMana();
}

float AWKPlayerState::GetManaRegenRate() const
{
	return AttributeSetBase->GetManaRegenRate();
}

float AWKPlayerState::GetStamina() const
{
	return AttributeSetBase->GetStamina();
}

float AWKPlayerState::GetMaxStamina() const
{
	return AttributeSetBase->GetMaxStamina();
}

float AWKPlayerState::GetStaminaRegenRate() const
{
	return AttributeSetBase->GetStaminaRegenRate();
}

float AWKPlayerState::GetHulu() const
{
	return AttributeSetBase->GetHulu();
}

float AWKPlayerState::GetMaxHulu() const
{
	return AttributeSetBase->GetMaxHulu();
}

float AWKPlayerState::GetHuluRegenRate() const
{
	return AttributeSetBase->GetHuluRegenRate();
}


float AWKPlayerState::GetArmor() const
{
	return AttributeSetBase->GetArmor();
}

float AWKPlayerState::GetMoveSpeed() const
{
	return AttributeSetBase->GetMoveSpeed();
}

int32 AWKPlayerState::GetCharacterLevel() const
{
	return AttributeSetBase->GetCharacterLevel();
}

int32 AWKPlayerState::GetXP() const
{
	return AttributeSetBase->GetXP();
}

int32 AWKPlayerState::GetXPBounty() const
{
	return AttributeSetBase->GetXPBounty();
}

int32 AWKPlayerState::GetGold() const
{
	return AttributeSetBase->GetGold();
}

int32 AWKPlayerState::GetGoldBounty() const
{
	return AttributeSetBase->GetGoldBounty();
}

void AWKPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	// Update floating status bar
	AWKPlayerCharacterBase* WKPlayerCharacter = Cast<AWKPlayerCharacterBase>(GetPawn());
	if (WKPlayerCharacter)
	{
		UWKFloatingStatusBarWidget* HeroFloatingStatusBar = WKPlayerCharacter->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
		}
	}

	// Update the HUD
	// Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint

	// If the player died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		if (WKPlayerCharacter)
		{
			WKPlayerCharacter->Die();
		}
	}
}

void AWKPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	float MaxHealth = Data.NewValue;

	// Update floating status bar
	AWKPlayerCharacterBase* WKPlayerCharacter = Cast<AWKPlayerCharacterBase>(GetPawn());
	if (WKPlayerCharacter)
	{
		UWKFloatingStatusBarWidget* HeroFloatingStatusBar = WKPlayerCharacter->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(GetHealth() / MaxHealth);
		}
	}

	// Update the HUD
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxHealth(MaxHealth);
		}
	}
}

void AWKPlayerState::HealthRegenRateChanged(const FOnAttributeChangeData& Data)
{
	float HealthRegenRate = Data.NewValue;

	// Update the HUD
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetHealthRegenRate(HealthRegenRate);
		}
	}
}

void AWKPlayerState::ManaChanged(const FOnAttributeChangeData& Data)
{
	float Mana = Data.NewValue;

	// Update floating status bar
	AWKPlayerCharacterBase* WKPlayerCharacter = Cast<AWKPlayerCharacterBase>(GetPawn());
	if (WKPlayerCharacter)
	{
		UWKFloatingStatusBarWidget* HeroFloatingStatusBar = WKPlayerCharacter->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetManaPercentage(Mana / GetMaxMana());
		}
	}

	// Update the HUD
	// Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
}

void AWKPlayerState::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	float MaxMana = Data.NewValue;

	// Update floating status bar
	AWKPlayerCharacterBase* WKPlayerCharacter = Cast<AWKPlayerCharacterBase>(GetPawn());
	if (WKPlayerCharacter)
	{
		UWKFloatingStatusBarWidget* HeroFloatingStatusBar = WKPlayerCharacter->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetManaPercentage(GetMana() / MaxMana);
		}
	}

	// Update the HUD
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxMana(MaxMana);
		}
	}
}

void AWKPlayerState::ManaRegenRateChanged(const FOnAttributeChangeData& Data)
{
	float ManaRegenRate = Data.NewValue;

	// Update the HUD
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetManaRegenRate(ManaRegenRate);
		}
	}
}

void AWKPlayerState::StaminaChanged(const FOnAttributeChangeData& Data)
{
	float Stamina = Data.NewValue;

	// Update the HUD
	// Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
}

void AWKPlayerState::MaxStaminaChanged(const FOnAttributeChangeData& Data)
{
	float MaxStamina = Data.NewValue;

	// Update the HUD
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxStamina(MaxStamina);
		}
	}
}

void AWKPlayerState::StaminaRegenRateChanged(const FOnAttributeChangeData& Data)
{
	float StaminaRegenRate = Data.NewValue;

	// Update the HUD
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetStaminaRegenRate(StaminaRegenRate);
		}
	}
}

void AWKPlayerState::HuluChanged(const FOnAttributeChangeData& Data)
{
	float Hulu = Data.NewValue;

	// Update the HUD
	// Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
}

void AWKPlayerState::MaxHuluChanged(const FOnAttributeChangeData& Data)
{
	float MaxHulu = Data.NewValue;

	// Update the HUD
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxHulu(MaxHulu);
		}
	}
}

void AWKPlayerState::HuluRegenRateChanged(const FOnAttributeChangeData& Data)
{
	float HuluRegenRate = Data.NewValue;

	// Update the HUD
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetHuluRegenRate(HuluRegenRate);
		}
	}
}

void AWKPlayerState::XPChanged(const FOnAttributeChangeData& Data)
{
	float XP = Data.NewValue;

	// Update the HUD
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetExperience(XP);
		}
	}
}

void AWKPlayerState::GoldChanged(const FOnAttributeChangeData& Data)
{
	float Gold = Data.NewValue;

	// Update the HUD
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetGold(Gold);
		}
	}
}

void AWKPlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	float CharacterLevel = Data.NewValue;

	// Update the HUD
	AWKPlayerController* PC = Cast<AWKPlayerController>(GetOwner());
	if (PC)
	{
		UWKWidgetHUD* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetHeroLevel(CharacterLevel);
		}
	}
}

void AWKPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}
