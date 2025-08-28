// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAbilityTypes.h"
#include "Abilities/GameplayAbility.h"
#include "WKGameplayAbility.generated.h"

class UWKGameplayAbilityDataAsset;
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

USTRUCT()
struct FWKGameplayAbilityEndInfoCache
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayAbilitySpecHandle Handle;

	UPROPERTY()
	FGameplayAbilityActivationInfo ActivationInfo;

	UPROPERTY()
	bool bReplicateEndAbility;

	UPROPERTY()
	bool bWasCancelled;
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

	/** 当前技能被优先级打断 */
	UFUNCTION()
	void AbilityInterruptedByPriority(UWKGameplayAbility* InterruptAbility);

	/** 当前技能被优先级打断 */
	UFUNCTION(BlueprintImplementableEvent)
	void K2_AbilityInterruptedByPriority(UWKGameplayAbility* InterruptAbility);

	// 检查是否需要打断技能 用于额外的优先级条件
	UFUNCTION(BlueprintNativeEvent)
	bool CheckPriorityActivateInterrupt(const UWKGameplayAbilityDataAsset* NewAbilityDataAsset) const;

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& AbilitySpec) const;
	EWKAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	UFUNCTION(BlueprintPure)
	AWKCharacterBase* GetWKCharacterFromActorInfo() const;

	UFUNCTION(BlueprintPure)
	UWKAbilitySystemComponent* GetWKAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable)
	void AddGameplayTag(FGameplayTag AddToTag);

	UFUNCTION(BlueprintCallable)
	void RemoveGameplayTag(FGameplayTag RemoveToTag);

	/** 是否是优先级GA */
	UFUNCTION(BlueprintPure)
	bool HasPriority() const;

	/** 优先级是否仅用于检查激活 */
	UFUNCTION(BlueprintPure)
	bool CheckPriorityActivateOnly() const;

public:
	FOnGameplayAbilityEnded OnPostAbilityEndDelegate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Skill")
	TObjectPtr<UWKGameplayAbilityDataAsset> AbilityDataAsset;

	// 服务器是否检查优先级，默认开启，在需要网络同步要求高时只本地检测
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKGameplayAbility")
	bool bServerCheckCanActive = true;

	// 是否严格以客户端优先级为准
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKGameplayAbility")
	bool bClientPriorityFirst = false;

	// 默认绑定的按键类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKGameplayAbility")
	EWKGameplayAbilityInputBinds DefaultInputBind;
	
protected:
	/// UGameplayAbility
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData = nullptr) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	/// ~UGameplayAbility
	
	virtual void PreEndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);
	
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "PreEndAbility", meta=(ScriptName = "PreEndAbility"))
	void K2_PreEndAbility(const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	virtual void OnAbilityActivated(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	virtual void OnAbilityEnded(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);
	virtual void OnPostAbilityEnded(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	UFUNCTION(BlueprintImplementableEvent, Category = "WKGameplayAbility")
	void K2_PostEndAbilityEnded(bool bWasCancelled);
	
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

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void K2_PreActivate(const FGameplayEventData& TriggerEventData);

	/** 检查是否可以激活技能，失败时注入输入缓存 */
	UFUNCTION(BlueprintNativeEvent)
	bool CheckPriorityActivateAbility(float RequestPriority, float CurrentPriority, const UWKGameplayAbilityDataAsset* NewAbilityDataAsset) const;

protected:
	/** 这个GA的激活策略 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WKConfig|Ability Activation")
	EWKAbilityActivationPolicy ActivationPolicy;

private:
	// 阻止 End Ability 的 Ability Task
	UPROPERTY()
	TWeakObjectPtr<UAbilityTask> TaskToBlockEndAbility;

	UPROPERTY()
	FWKGameplayAbilityEndInfoCache EndInfoCache;

	UPROPERTY()
	bool bWaitingTaskToEndAbility;

	bool bIsIndicator = false;
	
public:
	/** 设置一个AbilityTask来Block End Ability，由这个AbilityTask来控制 End：如果当Ability先End的时候，不进行End，而是等待Task End之后再 EndAbility */
	UFUNCTION(BlueprintCallable)
	bool SetAbilityTaskToBlockEndAbility(UAbilityTask* AbilityToSet);
	
	UFUNCTION(BlueprintCallable)
	void UnSetAbilityTaskToBlockEndAbility();
};
