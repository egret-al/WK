// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WKCharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLACKWK_API UWKCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UWKCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);
};
