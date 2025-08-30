// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimNotify.h"
#include "WKAnimNotify_ShouldMove.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAnimNotify_ShouldMove : public UWKAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
