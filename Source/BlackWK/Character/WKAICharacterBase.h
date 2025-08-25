// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WKCharacterBase.h"
#include "BlackWK/AI/WKAIStateInterface.h"
#include "BlackWK/AI/WKLockableInterface.h"
#include "WKAICharacterBase.generated.h"

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

	/// IWKAIStateInterface
	virtual bool HasSawEnemyTarget() override;
	/// ~IWKAIStateInterface

	/// IWKLockableInterface
	virtual FVector GetLockLocation_Implementation() override;
	/// ~IWKLockableInterface
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WK|UI")
	TSubclassOf<UWKFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
	TObjectPtr<UWKFloatingStatusBarWidget> UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "WK|UI")
	TObjectPtr<UWidgetComponent> UIFloatingStatusBarComponent;
};
