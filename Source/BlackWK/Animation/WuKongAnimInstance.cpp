// Fill out your copyright notice in the Description page of Project Settings.


#include "WuKongAnimInstance.h"

#include "BlackWK/Character/WuKongPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UWuKongAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwnerWK.IsValid())
	{
		OwnerWuKong = Cast<AWuKongPlayerCharacter>(OwnerWK);
	}
}

void UWuKongAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerWuKong.IsValid())
	{
		return;
	}
	
	// 移动速度
	FVector Velocity = OwnerWuKong->GetVelocity();
	VelocityPlane.X = Velocity.X;
	VelocityPlane.Y = Velocity.Y;
	VelocityPlaneLength = Velocity.Length();

	UpdateInput();
	UpdateRotation();
}

void UWuKongAnimInstance::NativePostEvaluateAnimation()
{
	if (bIgnoreMontageRootMotionRotation)
	{
		if(!GetSkelMeshComponent()->IsPlayingRootMotion())
		{
			RootRotationCache = FQuat::Identity;
		}
	}
}

void UWuKongAnimInstance::UpdateInput()
{
	MoveInput = OwnerWuKong->GetMoveInput();
}

void UWuKongAnimInstance::UpdateRotation()
{
	// 两种计算TurnAngle的方法等价
	FVector CurrentAcceleration = OwnerWuKong->GetCharacterMovement()->GetCurrentAcceleration();
	FVector LocalAcceleration = UKismetMathLibrary::LessLess_VectorRotator(CurrentAcceleration, OwnerWuKong->GetActorRotation());
	TurnAngle = UKismetMathLibrary::DegAtan2(LocalAcceleration.Y, LocalAcceleration.X);


	float CurrentAccelerationLength = CurrentAcceleration.Length();
	
	if (AccelerationLength + CurrentAccelerationLength == 0.f)
	{
		ContinuousEmptyAccelNum++;
	}
	else
	{
		ContinuousEmptyAccelNum = 0;
	}
	bStop = ContinuousEmptyAccelNum > MaxContinuousEmptyAccelNum;

	if (FMath::Abs(LastAccelerationLength - CurrentAccelerationLength) > 300.f && CurrentAccelerationLength > 1000.f)
	{
		bCanTurn = true;
	}
	else
	{
		if (bCanTurn)
		{
			if (CanTurnFrameWindow > 10)
			{
				bCanTurn = false;
				CanTurnFrameWindow = 0;
			}
			else
			{
				CanTurnFrameWindow++;
			}
		}
	}

	// CalcTurnAngleFinal()
	
	
	LastAccelerationLength = AccelerationLength;
	AccelerationLength = CurrentAccelerationLength;

	UpdateTurn180();

	FString Msg = FString::Printf(TEXT("%f, %f|  %d %d %d %d"), TurnAngle, TurnAngle180, bEnterTurnLeft180, bEnterTurnRight180, bRunStartL, bRunStartR);
	GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Red, Msg);
}

void UWuKongAnimInstance::OnEnterTurnLeft180()
{
	bEnterTurnLeft180 = true;
}

void UWuKongAnimInstance::OnEnterTurnRight180()
{
	bEnterTurnRight180 = true;
}

void UWuKongAnimInstance::ResetTurn180()
{
	bEnterTurnLeft180 = false;
	bEnterTurnRight180 = false;
}

void UWuKongAnimInstance::UpdateTurn180()
{
	// 保存在进入大旋转前的Transform信息，大旋转中使用TurnAngle180来更新动画
	FTransform AccelerationTransform;
	AccelerationTransform.SetLocation(OwnerWuKong->GetCharacterMovement()->GetCurrentAcceleration() + TurnTransform180.GetLocation());
	FTransform RelativeTransform = AccelerationTransform.GetRelativeTransform(TurnTransform180);
	TurnAngle180 = UKismetMathLibrary::DegAtan2(RelativeTransform.GetLocation().Y, RelativeTransform.GetLocation().X);

	const bool bAnyTurn180 = bEnterTurnLeft180 || bEnterTurnRight180;
	const bool bAnyRunStart = bRunStartL || bRunStartR;
	
	if (!bAnyTurn180 && !bAnyRunStart)
	{
		// 记录Transform
		TurnTransform180 = OwnerWuKong->GetTransform();
	}
}

void UWuKongAnimInstance::OnRunStartLeft()
{
	bRunStartL = true;
	bRunStartR = false;
}

void UWuKongAnimInstance::OnRunStartRight()
{
	bRunStartL = false;
	bRunStartR = true;
}

void UWuKongAnimInstance::ResetRunStart()
{
	bRunStartL = false;
	bRunStartR = false;
}

void UWuKongAnimInstance::CalcTurnAngleFinal(
	float InTurnAngle,
	float InTurnAngle1,
	float InTurnAngle2,
	float InTurnAngleL1,
	float InTurnAngleL2,
	float InTurnAngleR1,
	float InTurnAngleR2,
	int32 InRotationNum,
	bool bIsCircleL,
	bool bIsCircleR,
	float& OutTurnAngleL,
	float& OutTurnAngleR,
	float& OutTurnAngleFinal,
	int32& OutRotationNum,
	bool& bOutCircleL,
	bool& bOutCircleR)
{
}

void UWuKongAnimInstance::ModifyRootMotionTransform(FTransform& InoutTransform)
{
	// 提取Transform中的旋转，叠加到RootRotationCache
	if (bIgnoreMontageRootMotionRotation)
	{
		// 如果已缓存旋转，则需要调整位移
		if (!RootRotationCache.IsIdentity())
		{
			FVector Translation = InoutTransform.GetTranslation();
			Translation = RootRotationCache.RotateVector(Translation);
			InoutTransform.SetTranslation(Translation);
		}
		
		// 叠加旋转
		RootRotationCache = InoutTransform.GetRotation() * RootRotationCache;
		InoutTransform.SetRotation(FQuat::Identity);
	}
}
