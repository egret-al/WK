// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Abilities/WKAbilityTypes.h"
#include "DataAssets/WKGameplayAbilityDataAsset.h"
#include "WKAbilitySystemComponent.generated.h"

struct FWKGameplayAbilityPriorityInfo;
class UWKGameplayAbility;

DECLARE_MULTICAST_DELEGATE_OneParam(FWKAbilityInputDelegate, int32)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWKGameplayAbilityPriorityDelegate, float, OldPriority, float, NewPriority);

struct FAbilityInputListenerHandle
{
public:
	FGameplayAbilitySpecHandle AbilityHandle;
	TArray<bool> bListen;

	FAbilityInputListenerHandle(FGameplayAbilitySpecHandle Handle)
	{
		AbilityHandle = Handle;
		bListen.SetNumZeroed(EAbilityGenericReplicatedEvent::MAX);
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLACKWK_API UWKAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UWKAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "WKAbilitySystemComponent")
	bool HasMatchingGameplayTagExactly(FGameplayTag TagToCheck) const;
	
	/** Ability自定义监听输入事件 */
	void AbilityListenWithInput(int32 InputID, FGameplayAbilitySpecHandle AbilityHandle, EAbilityGenericReplicatedEvent::Type ListenType);
	/** Ability自定义取消监听输入事件 */
	void AbilityCancelListenWithInput(int32 InputID, FGameplayAbilitySpecHandle AbilityHandle, EAbilityGenericReplicatedEvent::Type ListenType);
	
	virtual void AbilityLocalInputPressed(int32 InputID) override;
	virtual void AbilityLocalInputReleased(int32 InputID) override;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable)
	void AddGameplayTag(FGameplayTag AddToTag);

	UFUNCTION(BlueprintCallable)
	void RemoveGameplayTag(FGameplayTag RemoveTag);

	FSimpleMulticastDelegate& WKAbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::Type EventType, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey);

	UFUNCTION(BlueprintCallable)
	void UpdateCurrentPriorityAbility(UWKGameplayAbility* RequestAbility, const FWKGameplayAbilityPriorityInfo& PriorityInfo);
	
	UFUNCTION(BlueprintPure)
	UWKGameplayAbility* GetCurrentPriorityAbility() const;

	UFUNCTION(BlueprintPure)
	FWKGameplayAbilityPriorityInfo GetCurrentPriorityInfo() const;
	
	UFUNCTION()
	void ApplyPriorityBlockAndCancel(UWKGameplayAbility* RequestAbility, const FWKGameplayAbilityPriorityInfo& PriorityInfo, bool bEndAbility);

	void SetCurrentPriorityAbilityTime(float Time) { CurrentPriorityAbilityTime = Time; }

	UFUNCTION(BlueprintPure)
	float GetCurrentPriorityAbilityTime() const { return CurrentPriorityAbilityTime; }

	/** Replicates targeting data to the client */
	UFUNCTION(Client, reliable, WithValidation)
	void ClientSetReplicatedTargetDataEx(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid, const FGameplayAbilityTargetDataHandle& TargetDataHandle, FPredictionKey CurrentPredictionKey);

	/** 复制 targeting data to the server */
	UFUNCTION(Server, reliable, WithValidation)
	void ServerSetReplicatedTargetDataEx(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid, const FGameplayAbilityTargetDataHandle& TargetDataHandle, FPredictionKey CurrentPredictionKey);

	// 根据技能和节点GUID，设置TargetData被设置的Delegate
	FDelegateHandle SetReplicatedTargetDataDelegateEx(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid, FAbilityTargetDataSetDelegate::FDelegate&& InDelegate);
	void RemoveReplicatedTargetDataDelegateEx(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid, FDelegateHandle DelegateHandle);

	// 如果数据已被设置，则触发Delegate
	bool CallReplicatedTargetDataDelegatesIfSetEx(FGameplayAbilitySpecHandle AbilityHandle, const FGuid& Guid);

	// 根据技能和节点GUID，清除缓存的数据与回调
	void ClearAbilityReplicatedDataCacheEx(FGameplayAbilitySpecHandle Handle, const FGuid& Guid);
	void ClearAbilityReplicatedDataCacheEx(FGameplayAbilitySpecHandle Handle);

protected:
	/** 尝试在生成时就去激活可以激活的GA */
	void TryActivateAbilitiesOnSpawn();

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

public:
	FWKAbilityInputDelegate OnAbilityInputPressedDelegate;
	FWKAbilityInputDelegate OnAbilityInputReleaseDelegate;

	UPROPERTY(BlueprintAssignable)
	FWKGameplayAbilityPriorityDelegate OnAbilityPriorityUpdateDelegate;

protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	TMap<FGameplayAbilityGuidKey, FWKAbilityReplicatedDataCache> AbilityTargetDataMapEx;
	
private:
	UPROPERTY()
	float LastPriorityAbilityTime;

	UPROPERTY()
	float CurrentPriorityAbilityTime;
	
	UPROPERTY()
	TObjectPtr<UWKGameplayAbility> LastPriorityAbility;
	
	// 当前激活中的优先级GA，没有时为空
	UPROPERTY()
	TObjectPtr<UWKGameplayAbility> CurrentPriorityAbility;

	// 优先级打断相关信息
	UPROPERTY()
	FWKGameplayAbilityPriorityInfo CurrentPriorityInfo;

	TMap<int32, TArray<FAbilityInputListenerHandle>> InputListeners;
};
