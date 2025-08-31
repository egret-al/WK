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
	UWKAbilitySystemComponent* GetWKAbilitySystemComponent() const;

	void SetPawnData(const UWKPawnData* InPawnData);

	template<typename T>
	const T* GetPawnData() const
	{
		return Cast<T>(PawnData);
	}

	UFUNCTION(BlueprintCallable, Category = "WKPlayerState")
	bool IsAlive() const;

	UFUNCTION(BlueprintPure)
	float GetHealth() const;

	UFUNCTION(BlueprintPure)
	float GetMaxHealth() const;

protected:
	UFUNCTION()
	void OnRep_PawnData();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UWKPawnData> PawnData;
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "ASC")
	TObjectPtr<UWKAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "ASC")
	TObjectPtr<UWKHealthSet> HealthSet;

	UPROPERTY(VisibleAnywhere, Category = "ASC")
	TObjectPtr<UWKCombatSet> CombatSet;
};
