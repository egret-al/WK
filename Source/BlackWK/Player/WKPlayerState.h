// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "WKPlayerState.generated.h"

class UWKPawnData;
class UWKCombatSet;
class UWKHealthSet;
struct FOnAttributeChangeData;
class UWKAttributeSetBase;
class UWKAbilitySystemComponent;

UCLASS()
class BLACKWK_API AWKPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AWKPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void SetPawnData(const UWKPawnData* InPawnData);

	template<typename T>
	const T* GetPawnData() const
	{
		return Cast<T>(PawnData);
	}

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|UI")
	void ShowAbilityConfirmCancelText(bool ShowText);

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetHealthRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetManaRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetStaminaRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetHulu() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetMaxHulu() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetHuluRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetArmor() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	int32 GetXP() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	int32 GetXPBounty() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	int32 GetGold() const;

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState|Attributes")
	int32 GetGoldBounty() const;

protected:
	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void HealthRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	virtual void ManaRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void StaminaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxStaminaChanged(const FOnAttributeChangeData& Data);
	virtual void StaminaRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void HuluChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHuluChanged(const FOnAttributeChangeData& Data);
	virtual void HuluRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void XPChanged(const FOnAttributeChangeData& Data);
	virtual void GoldChanged(const FOnAttributeChangeData& Data);
	virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);

	// Tag change callbacks
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
protected:
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle HealthRegenRateChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle ManaRegenRateChangedDelegateHandle;
	FDelegateHandle StaminaChangedDelegateHandle;
	FDelegateHandle MaxStaminaChangedDelegateHandle;
	FDelegateHandle StaminaRegenRateChangedDelegateHandle;
	FDelegateHandle HuluChangedDelegateHandle;
	FDelegateHandle MaxHuluChangedDelegateHandle;
	FDelegateHandle HuluRegenRateChangedDelegateHandle;
	FDelegateHandle XPChangedDelegateHandle;
	FDelegateHandle GoldChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;

protected:
	UFUNCTION()
	void OnRep_PawnData();

protected:
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UWKPawnData> PawnData;
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "ASC")
	TObjectPtr<UWKAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "ASC")
	TObjectPtr<UWKHealthSet> HealthSet;

	UPROPERTY(VisibleAnywhere, Category = "ASC")
	TObjectPtr<UWKCombatSet> CombatSet;
};
