// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WKHealthComponent.generated.h"

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

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthNormalized() const;

public:
	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FWKHealth_AttributeChanged OnMaxHealthChanged;

protected:
	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

protected:
	UPROPERTY()
	TObjectPtr<UWKAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<const UWKHealthSet> HealthSet;
};
