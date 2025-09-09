// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WKHealthComponent.generated.h"

class UWKCombatSet;
struct FGameplayEffectSpec;
class UWKHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWKHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FWKHealth_AttributeChanged, UWKHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

class UWKAbilitySystemComponent;
class UWKHealthSet;

UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class BLACKWK_API UWKHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWKHealthComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "Health")
	static UWKHealthComponent* FindHealthComponent(const AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void InitializeWithAbilitySystem(UWKAbilitySystemComponent* InASC);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void UninitializeFromAbilitySystem();

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetStamina() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetResilience() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxResilience() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMana() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxMana() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetStickEnergy() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxStickEnergy() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthNormalized() const;

public:
	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnManaChanged;

	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnResilienceChanged;

	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnMaxResilienceChanged;

	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnMaxStaminaChanged;

	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnStickEnergyChanged;

	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnMaxStickEnergyChanged;

protected:
	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleManaChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxManaChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleResilienceChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxResilienceChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleStaminaChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxStaminaChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleStickEnergyChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxStickEnergyChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

protected:
	UPROPERTY()
	TObjectPtr<UWKAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<const UWKHealthSet> HealthSet;

	UPROPERTY()
	TObjectPtr<const UWKCombatSet> CombatSet;
};
