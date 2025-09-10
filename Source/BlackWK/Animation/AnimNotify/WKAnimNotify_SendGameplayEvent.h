// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WKAnimNotify.h"
#include "WKAnimNotify_SendGameplayEvent.generated.h"

UCLASS()
class BLACKWK_API UWKAnimNotify_SendGameplayEvent : public UWKAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig")
	FGameplayTag EventTag;
};
