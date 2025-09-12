// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimNotify.h"
#include "WKAnimNotify_SetGlobalTimeDilation.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAnimNotify_SetGlobalTimeDilation : public UWKAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WKConfig", meta = (ClampMin = 0.01, ClampMax = 1.f))
	float GlobalTimeDilation;
};
