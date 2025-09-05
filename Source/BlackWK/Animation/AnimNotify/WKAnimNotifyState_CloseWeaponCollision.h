// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimNotifyState.h"
#include "WKAnimNotifyState_CloseWeaponCollision.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAnimNotifyState_CloseWeaponCollision : public UWKAnimNotifyState
{
	GENERATED_BODY()

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig")
	TArray<FName> BoneNames;
};
