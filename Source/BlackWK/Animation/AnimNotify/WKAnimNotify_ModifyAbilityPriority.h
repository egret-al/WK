// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimNotify.h"
#include "BlackWK/AbilitySystem/DataAssets/WKGameplayAbilityDataAsset.h"
#include "WKAnimNotify_ModifyAbilityPriority.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAnimNotify_ModifyAbilityPriority : public UWKAnimNotify
{
	GENERATED_BODY()

public:
	UWKAnimNotify_ModifyAbilityPriority();
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	virtual FString GetNotifyName_Implementation() const override;

public:
	// 当前激活的优先级GA的最新优先级信息
	UPROPERTY(EditAnywhere, Category = "WKConfig|Priority")
	FWKGameplayAbilityPriorityInfo AbilityPriorityInfo;

	UPROPERTY(EditAnywhere, Category = "WKConfig|Priority")
	FGameplayTag SkillTag;
};
