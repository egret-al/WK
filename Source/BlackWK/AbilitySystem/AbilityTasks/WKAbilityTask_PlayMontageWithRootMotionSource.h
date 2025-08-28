// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAbilityTask_PlayMontageAndWaitForEvent.h"
#include "BlackWK/Animation/AnimNotify/WKAnimNotifyState_RootMotionWarping.h"
#include "BlackWK/Animation/RootMotionSource/WKRootMotionSource_RootMotionWarping.h"
#include "WKAbilityTask_PlayMontageWithRootMotionSource.generated.h"

class UCharacterMovementComponent;

/**
 * 此方法需要Owner是Character的子类，因为RootMotionSource需要有CharacterMovement组件
 * Owner不是DSCharacter的子类请使用PlayMontageAndWaitForEvent
 *
 * 由于GAS的蒙太奇同步机制只能同步一个蒙太奇的状态，所以在同一帧（严谨点说是同一个引擎变量同步周期内）只能调用一次PlayMontageWithRootMotionSource
 * 
 * 连续调用（有时间间隔）该节点播放蒙太奇时，后一个蒙太奇播放时会自动停止上一个蒙太奇(无论蒙太奇的Slot是否是同一个)，如果不需要自动停止，请在蒙太奇资产的MetaData中添加NotAutoStop，可自行调用ADSCharacterBase::StopMontageRpc进行停止
 */
UCLASS()
class BLACKWK_API UWKAbilityTask_PlayMontageWithRootMotionSource : public UWKAbilityTask_PlayMontageAndWaitForEvent
{
	GENERATED_BODY()

public:
	UWKAbilityTask_PlayMontageWithRootMotionSource(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 此方法需要Owner是DSCharacter的子类（后续DSCharacter拆分之后会换，但至少是Character的子类，因为RootMotionSource需要有CharacterMovement组件）Owner不是DSCharacter的子类请使用PlayMontageAndWaitForEvent */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility, AutoGuid", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE", AutoCreateRefTerm = "AnimationWarpingParams"))
	static UWKAbilityTask_PlayMontageWithRootMotionSource* PlayMontageWithRootMotionSource(
		UGameplayAbility* OwningAbility,
		const FGuid AutoGuid,
		FName TaskInstanceName,
		UAnimMontage* Montage,
		int32 SectionIndex,
		float SectionStartTime ,
		FGameplayTagContainer WaitEventTags,
		float PlayRate = 1.f,
		float RootMotionTranslationScale = 1.f,
		bool bStopWhenAbilityEnd = true);

	virtual void Activate() override;
	virtual void InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	virtual void PreDestroyFromReplication() override;

protected:
	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);
	
	UFUNCTION()
	void OnRep_ActorRotator();

	void SharedInitAndApply();
	const bool NeedReplicateAutonomousClientParamToServer();
	void PrepareReplicateParams();
	bool HandleRootMotionWarping();

	static void ArrangeNotifySection(TArray<FRootMotionWarpingSection_Translation>& WarpingSections_Translations,
		TArray<FRootMotionWarpingSection_Rotation>& WarpingSections_Rotations,
		TArray<FRootMotionWarpingSection_TargetActor>& WarpingSections_TargetActors,
		TArray<FRootMotionWarpingSection_Location>& WarpingSections_Locations);

	template<typename T>
	static void SortNotifySection(TArray<T>& WarpingSections);

	template<typename T>
	static void SortNotifySectionPointer(TArray<T>& WarpingSections);

	template<typename T>
	static void CutNotifySection(const FRootMotionWarpingSection_TargetActor& TargetActorSection, TArray<T>& CutSections);
	
protected:
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(Replicated)
	int32 SectionIndex;

	UPROPERTY(Replicated,ReplicatedUsing=OnRep_ActorRotator)
	FRotator ActorRotator;
	
	UPROPERTY(Replicated)
	FRotator ControlRotator;
	
	UPROPERTY(Replicated)
	FVector_NetQuantize100 CharacterMoveInput;

	UPROPERTY(Replicated)
	TObjectPtr<AActor> PlayerTarget;

	UPROPERTY(Replicated)
	TArray<float> OverrideWarpingDistance;

	UPROPERTY(Replicated)
	TArray<FVector_NetQuantize100> OverrideWarpingDirection;
	
	UPROPERTY(Replicated)
	TArray<FVector_NetQuantize100> CustomTargetLocation;

	UPROPERTY(Replicated)
	TArray<float> OverrideWarpingRotation;

	UPROPERTY(Replicated)
	FWKOverrideWarpingTargetActorParam OverrideWarpingTargetActorParam;

	UPROPERTY(Replicated)
	bool bContinueRootMotionWhenMontageStop;

	FDelegateHandle CallBackHandle;
	uint16 RootMotionSourceID;
};

template <typename T>
void UWKAbilityTask_PlayMontageWithRootMotionSource::SortNotifySection(TArray<T>& WarpingSections)
{
	// 后开始的AnimNotifyState覆盖之前的
	for (int32 i = 1; i < WarpingSections.Num(); i++)
	{
		if (WarpingSections[i].StartTime < WarpingSections[i - 1].EndTime)
		{
			WarpingSections[i - 1].EndTime = WarpingSections[i].StartTime;
		}
	}
}

template <typename T>
void UWKAbilityTask_PlayMontageWithRootMotionSource::SortNotifySectionPointer(TArray<T>& WarpingSections)
{
	// 后开始的AnimNotifyState覆盖之前的
	for (int32 i = 1; i < WarpingSections.Num(); i++)
	{
		if (WarpingSections[i]->StartTime < WarpingSections[i - 1]->EndTime)
		{
			WarpingSections[i - 1]->EndTime = WarpingSections[i]->StartTime;
		}
	}
}

template <typename T>
void UWKAbilityTask_PlayMontageWithRootMotionSource::CutNotifySection(const FRootMotionWarpingSection_TargetActor& TargetActorSection, TArray<T>& CutSections)
{
	TArray<T> NewSections;
	for (T& Section : CutSections)
	{
		if (TargetActorSection.EndTime >= Section.StartTime && TargetActorSection.StartTime <= Section.EndTime)
		{
			if (TargetActorSection.StartTime <= Section.StartTime
				&& TargetActorSection.EndTime >= Section.EndTime)
			{
				//全部覆盖，直接删除
				continue;
			}
			else if (TargetActorSection.EndTime <= Section.EndTime)
			{
				Section.StartTime = TargetActorSection.EndTime;
				NewSections.Add(Section);
			}
			else if (TargetActorSection.StartTime >= Section.StartTime)
			{
				Section.EndTime = TargetActorSection.StartTime;
				NewSections.Add(Section);
			}
		}
		else
		{
			NewSections.Add(Section);
		}
	}
	CutSections = MoveTemp(NewSections);
}
