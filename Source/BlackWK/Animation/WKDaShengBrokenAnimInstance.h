// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKAnimInstanceBase.h"
#include "WKDaShengBrokenAnimInstance.generated.h"

class AWKAIDaShengCharacter;

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKDaShengBrokenAnimInstance : public UWKAnimInstanceBase
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "References")
	TWeakObjectPtr<AWKAIDaShengCharacter> OwnerDaSheng;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	bool bHasSawTarget = false;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	TWeakObjectPtr<AWKCharacterBase> CurrentTarget;
};
