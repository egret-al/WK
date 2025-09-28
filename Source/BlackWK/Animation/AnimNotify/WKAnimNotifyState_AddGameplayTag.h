// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WKAnimNotifyState.h"
#include "WKAnimNotifyState_AddGameplayTag.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAnimNotifyState_AddGameplayTag : public UWKAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig")
	FGameplayTag AddToTag;
};
