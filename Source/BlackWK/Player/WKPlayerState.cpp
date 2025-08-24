// Fill out your copyright notice in the Description page of Project Settings.


#include "WKPlayerState.h"

#include "WKPlayerController.h"
#include "BlackWK/AbilitySystem/WKAbilitySet.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/AttributeSets/WKHealthSet.h"
#include "BlackWK/AbilitySystem/AttributeSets/WKCombatSet.h"
#include "BlackWK/Character/WKPawnData.h"
#include "BlackWK/Character/WKPlayerCharacterBase.h"
#include "BlackWK/UI/WKFloatingStatusBarWidget.h"
#include "BlackWK/UI/WKWidgetHUD.h"
#include "Net/UnrealNetwork.h"

AWKPlayerState::AWKPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UWKAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	NetUpdateFrequency = 100.0f;

	HealthSet = CreateDefaultSubobject<UWKHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UWKCombatSet>(TEXT("CombatSet"));
}

void AWKPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
}

void AWKPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetHealthAttribute()).AddUObject(this, &AWKPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetMaxHealthAttribute()).AddUObject(this, &AWKPlayerState::MaxHealthChanged);
		HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetHealthRegenRateAttribute()).AddUObject(this, &AWKPlayerState::HealthRegenRateChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetManaAttribute()).AddUObject(this, &AWKPlayerState::ManaChanged);
		MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetMaxManaAttribute()).AddUObject(this, &AWKPlayerState::MaxManaChanged);
		ManaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetManaRegenRateAttribute()).AddUObject(this, &AWKPlayerState::ManaRegenRateChanged);
		StaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetStaminaAttribute()).AddUObject(this, &AWKPlayerState::StaminaChanged);
		MaxStaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetMaxStaminaAttribute()).AddUObject(this, &AWKPlayerState::MaxStaminaChanged);
		StaminaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetStaminaRegenRateAttribute()).AddUObject(this, &AWKPlayerState::StaminaRegenRateChanged);
		HuluChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetHuluAttribute()).AddUObject(this, &AWKPlayerState::HuluChanged);
		MaxHuluChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetMaxHuluAttribute()).AddUObject(this, &AWKPlayerState::MaxHuluChanged);
		HuluRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetHuluRegenRateAttribute()).AddUObject(this, &AWKPlayerState::HuluRegenRateChanged);
		XPChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetXPAttribute()).AddUObject(this, &AWKPlayerState::XPChanged);
		GoldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetGoldAttribute()).AddUObject(this, &AWKPlayerState::GoldChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetCharacterLevelAttribute()).AddUObject(this, &AWKPlayerState::CharacterLevelChanged);

		// Tag change callbacks
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AWKPlayerState::StunTagChanged);
	}

	if (HasAuthority())
	{
		//TODO
		SetPawnData(PawnData);
	}
}

UAbilitySystemComponent* AWKPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AWKPlayerState::SetPawnData(const UWKPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	// if (PawnData)
	// {
	// 	return;
	// }

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	if (GetNetMode() == NM_Standalone)
	{
		OnRep_PawnData();
	}

	for (const TObjectPtr<UWKAbilitySet> AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet.Get())
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
	ForceNetUpdate();
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
	return HealthSet->GetHealth();
}

float AWKPlayerState::GetMaxHealth() const
{
	return HealthSet->GetMaxHealth();
}

float AWKPlayerState::GetHealthRegenRate() const
{
	return HealthSet->GetHealthRegenRate();
}

float AWKPlayerState::GetMana() const
{
	return HealthSet->GetMana();
}

float AWKPlayerState::GetMaxMana() const
{
	return HealthSet->GetMaxMana();
}

float AWKPlayerState::GetManaRegenRate() const
{
	return HealthSet->GetManaRegenRate();
}

float AWKPlayerState::GetStamina() const
{
	return HealthSet->GetStamina();
}

float AWKPlayerState::GetMaxStamina() const
{
	return HealthSet->GetMaxStamina();
}

float AWKPlayerState::GetStaminaRegenRate() const
{
	return HealthSet->GetStaminaRegenRate();
}

float AWKPlayerState::GetHulu() const
{
	return HealthSet->GetHulu();
}

float AWKPlayerState::GetMaxHulu() const
{
	return HealthSet->GetMaxHulu();
}

float AWKPlayerState::GetHuluRegenRate() const
{
	return HealthSet->GetHuluRegenRate();
}


float AWKPlayerState::GetArmor() const
{
	return HealthSet->GetArmor();
}

float AWKPlayerState::GetMoveSpeed() const
{
	return HealthSet->GetMoveSpeed();
}

int32 AWKPlayerState::GetCharacterLevel() const
{
	return HealthSet->GetCharacterLevel();
}

int32 AWKPlayerState::GetXP() const
{
	return HealthSet->GetXP();
}

int32 AWKPlayerState::GetXPBounty() const
{
	return HealthSet->GetXPBounty();
}

int32 AWKPlayerState::GetGold() const
{
	return HealthSet->GetGold();
}

int32 AWKPlayerState::GetGoldBounty() const
{
	return HealthSet->GetGoldBounty();
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
	// if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	// {
	// 	if (WKPlayerCharacter)
	// 	{
	// 		WKPlayerCharacter->Die();
	// 	}
	// }
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

void AWKPlayerState::OnRep_PawnData()
{
}
