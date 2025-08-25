// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "WKAttributeSetBase.h"
#include "WKHealthSet.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WK_Damage_Message);

UCLASS()
class BLACKWK_API UWKHealthSet : public UWKAttributeSetBase
{
	GENERATED_BODY()

public:
	UWKHealthSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ATTRIBUTE_ACCESSORS(UWKHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UWKHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UWKHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UWKHealthSet, Damage);

	mutable FWKAttributeEvent OnHealthChanged;
	mutable FWKAttributeEvent OnMaxHealthChanged;
	mutable FWKAttributeEvent OnOutOfHealth;

protected:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	
private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "HealthSet|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "HealthSet|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	bool bOutOfHealth;
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;
	
	UPROPERTY(BlueprintReadOnly, Category="HealthSet|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;

	UPROPERTY(BlueprintReadOnly, Category="HealthSet|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;
};
