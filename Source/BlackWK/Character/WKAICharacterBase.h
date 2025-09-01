// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKCharacterBase.h"
#include "BlackWK/AI/WKAIStateInterface.h"
#include "BlackWK/AI/WKLockableInterface.h"
#include "WKAICharacterBase.generated.h"

class UBlackboardComponent;
class UWidgetComponent;
struct FOnAttributeChangeData;
class UWKFloatingStatusBarWidget;

UCLASS()
class BLACKWK_API AWKAICharacterBase : public AWKCharacterBase, public IWKAIStateInterface, public IWKLockableInterface
{
	GENERATED_BODY()

public:
	AWKAICharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	/// AWKCharacterBase
	virtual bool IsPlayerCharacter() override;
	virtual AWKCharacterBase* GetCurrentAttackLockTarget() const override;
	/// ~AWKCharacterBase

	/// IWKAIStateInterface
	virtual bool HasSawEnemyTarget() override;
	/// ~IWKAIStateInterface

	/// IWKLockableInterface
	virtual FVector GetLockLocation_Implementation() override;
	/// ~IWKLockableInterface
	
protected:
	UPROPERTY()
	TObjectPtr<UBlackboardComponent> BlackboardComponent;
};
