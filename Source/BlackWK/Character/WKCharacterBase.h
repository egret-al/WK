// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "WKCharacterAnimInfoInterface.h"
#include "GameFramework/Character.h"
#include "WKCharacterBase.generated.h"

class UGameplayEffect;
class UWKGameplayAbility;
class UWKAttributeSetBase;
class UWKAbilitySystemComponent;
class UWKHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AWKCharacterBase*, Character);

UCLASS()
class BLACKWK_API AWKCharacterBase : public ACharacter, public IAbilitySystemInterface, public IWKCharacterAnimInfoInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	AWKCharacterBase(const class FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	/// IWKCharacterAnimInfoInterface
	virtual FWKEssentialValue GetEssentialValues() override;
	/// ~IWKCharacterAnimInfoInterface

	/// IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	/// ~IGameplayTagAssetInterface
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UWKAbilitySystemComponent* GetWKAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PlayHitMontage(AWKCharacterBase* InInstigator);

	UFUNCTION(BlueprintCallable)
	void ClearMeleeComboIndex();

	UFUNCTION(BlueprintCallable)
	void SetMeleeComboIndex(int32 Index);

protected:
	virtual void OnAbilitySystemInitialized();

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

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UWKAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WKCharacter|Character")
	TObjectPtr<UWKHealthComponent> HealthComponent;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Combat", Replicated)
	int32 CurrentMeleeComboIndex = INDEX_NONE;

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
