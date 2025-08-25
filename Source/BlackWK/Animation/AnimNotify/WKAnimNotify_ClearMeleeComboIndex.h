// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimNotify.h"
#include "WKAnimNotify_ClearMeleeComboIndex.generated.h"

/**
 * 清空近战连击的索引
 */
UCLASS()
class BLACKWK_API UWKAnimNotify_ClearMeleeComboIndex : public UWKAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
