// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotifyState_FaceToTarget.h"

#include "BlackWK/Character/WKCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

void UWKAnimNotifyState_FaceToTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	if (!MeshComp)
	{
		return;
	}

	AWKCharacterBase* Owner = Cast<AWKCharacterBase>(MeshComp->GetOwner());
	if (!Owner)
	{
		return;
	}

	AWKCharacterBase* CurrentTarget = Owner->GetCurrentAttackLockTarget();
	if (!CurrentTarget)
	{
		return;
	}

	FVector OwnerLoc = Owner->GetActorLocation();
	FVector TargetLoc = CurrentTarget->GetActorLocation();

	// 只考虑 Yaw
	FRotator CurrentRot = Owner->GetActorRotation();
	FRotator DesiredRot = UKismetMathLibrary::FindLookAtRotation(OwnerLoc, TargetLoc);

	// 计算差值角度
	float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, DesiredRot.Yaw);

	// 限制旋转速度
	float MaxStep = MaxYawRate * FrameDeltaTime;
	float MinStep = MinYawRate * FrameDeltaTime;

	// Clamp 步长
	float Step = FMath::Clamp(DeltaYaw, -MaxStep, MaxStep);

	// 保证不会小于最小速度（除非已经接近目标角度）
	if (FMath::Abs(Step) < MinStep && FMath::Abs(DeltaYaw) > MinStep)
	{
		Step = (DeltaYaw > 0 ? 1 : -1) * MinStep;
	}

	// 应用旋转
	float NewYaw = CurrentRot.Yaw + Step;
	Owner->SetActorRotation(FRotator(0.f, NewYaw, 0.f));
}
