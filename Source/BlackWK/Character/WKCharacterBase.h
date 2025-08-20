// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "WKCharacterAnimInfoInterface.h"
#include "BlackWK/AbilitySystem/Abilities/WKAbilityTypes.h"
#include "GameFramework/Character.h"
#include "WKCharacterBase.generated.h"

class UGameplayEffect;
class UWKGameplayAbility;
class UWKAttributeSetBase;
class UWKAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AWKCharacterBase*, Character);

UCLASS()
class BLACKWK_API AWKCharacterBase : public ACharacter, public IAbilitySystemInterface, public IWKCharacterAnimInfoInterface
{
	GENERATED_BODY()

public:
	AWKCharacterBase(const class FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	/// IWKCharacterAnimInfoInterface
	virtual FWKEssentialValue GetEssentialValues() override;
	/// ~IWKCharacterAnimInfoInterface

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void RemoveCharacterAbilities();
	virtual void Die();
	
	UFUNCTION(BlueprintCallable)
	virtual void FinishDying();

	UFUNCTION(BlueprintCallable)
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "WKCharacter")
	virtual int32 GetAbilityLevel(EWKAbilityInputID AbilityID) const;

	UFUNCTION(BlueprintCallable, Category = "WKCharacter|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "WKCharacter|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "WKCharacter|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "WKCharacter|Attributes")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "WKCharacter|Attributes")
	float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "WKCharacter|Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "WKCharacter|Attributes")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "WKCharacter|Attributes")
	float GetHulu() const;

	UFUNCTION(BlueprintCallable, Category = "WKCharacter|Attributes")
	float GetMaxHulu() const;
	
	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "WKCharacter|Attributes")
	float GetMoveSpeed() const;

	// Gets the Base value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "WKCharacter|Attributes")
	float GetMoveSpeedBaseValue() const;

public:
	UPROPERTY(BlueprintAssignable)
	FCharacterDiedDelegate OnCharacterDiedDelegate;

protected:
	virtual void AddCharacterAbilities();
	virtual void InitializeAttributes();
	virtual void AddStartupEffects();
	
	virtual void SetHealth(float Health);
	virtual void SetMana(float Mana);
	virtual void SetStamina(float Stamina);
	virtual void SetHulu(float Hulu);
	
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "WKCharacter")
	FText CharacterName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WKCharacter|Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "WKCharacter|Abilities")
	TArray<TSubclassOf<UWKGameplayAbility>> CharacterAbilities;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "WKCharacter|Abilities")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "WKCharacter|Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;
	
	TWeakObjectPtr<UWKAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UWKAttributeSetBase> AttributeSetBase;

protected:
	virtual void OnBeginPlay();
	virtual void SetEssentialValues();
	virtual void CacheValues();
	FVector CalculateAcceleration() const;
	
private:
	FVector PreviousVelocity;
	float PreviousAimYaw = 0.f;
	float AimYawRate = 0.f;
	FVector Acceleration;
	float Speed = 0.f;
	bool bIsMoving = false;
	FRotator TargetRotation;
	FRotator LastVelocityRotation;
	FRotator LastMovementInputRotation;
	float MovementInputAmount = 0.f;
	bool bHasMovementInput = false;
};
