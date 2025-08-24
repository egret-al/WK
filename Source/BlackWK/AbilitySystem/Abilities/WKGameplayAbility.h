// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAbilityTypes.h"
#include "Abilities/GameplayAbility.h"
#include "WKGameplayAbility.generated.h"

class UWKAbilitySystemComponent;
class AWKCharacterBase;
/**
 * 定义如何去激活这个GA
 */
UENUM(BlueprintType)
enum class EWKAbilityActivationPolicy : uint8
{
	// 当输入是trigger时尝试激活
	OnInputTriggered,

	// 在输入激活期间持续激活这个GA
	WhileInputActive,
	
	// 当Avatar分配完毕时立即激活
	OnSpawn
};

/**
 * 
 */
UCLASS(Abstract)
class BLACKWK_API UWKGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

	friend class UWKAbilitySystemComponent;

public:
	UWKGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& AbilitySpec) const;
	EWKAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	UFUNCTION(BlueprintCallable)
	AWKCharacterBase* GetWKCharacterFromActorInfo() const;

	UFUNCTION(BlueprintPure)
	UWKAbilitySystemComponent* GetWKAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable)
	void AddGameplayTag(FGameplayTag AddToTag);

	UFUNCTION(BlueprintCallable)
	void RemoveGameplayTag(FGameplayTag RemoveToTag);
	
protected:
	/// UGameplayAbility
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	/// ~UGameplayAbility

	/** 当新的Pawn Avatar设置上后会被调用 */
	virtual void OnPawnAvatarSet();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void K2_OnAbilityAdded();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void K2_OnAbilityRemoved();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void K2_OnPawnAvatarSet();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void K2_InputPressed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void K2_InputReleased();

protected:
	/** 这个GA的激活策略 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Ability Activation")
	EWKAbilityActivationPolicy ActivationPolicy;
};
