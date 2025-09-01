// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WKAnimationFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAnimationFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void DrawDebugSkeletalMeshPose(const USkeletalMeshComponent* MeshComp, EBoneSpaces::Type Space = EBoneSpaces::WorldSpace, float Duration = 0.f, FColor Color = FColor::Green);

	static TArray<const FAnimNotifyEvent*> GetNotifyEventsFromAnimMontage(const UAnimMontage* AnimMontage, const UClass* NotifyClass, const int32 SectionIndex = INDEX_NONE);
};
