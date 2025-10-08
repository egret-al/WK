// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKBTService.h"
#include "WKBTService_UpdateIsPlayingMontage.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKBTService_UpdateIsPlayingMontage : public UWKBTService
{
	GENERATED_BODY()

public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// 当前正在播放蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WKConfig|Write")
	FBlackboardKeySelector IsPlayingMontageSelector;
};
