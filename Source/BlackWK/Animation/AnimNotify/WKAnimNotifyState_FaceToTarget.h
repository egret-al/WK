// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "WKAnimNotifyState_FaceToTarget.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKAnimNotifyState_FaceToTarget : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

protected:
	// 可以通过动画蒙太奇里设置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotate")
	float MinYawRate = 30.f;  // 每秒最小旋转速度（度/秒）

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotate")
	float MaxYawRate = 180.f; // 每秒最大旋转速度（度/秒）
};
