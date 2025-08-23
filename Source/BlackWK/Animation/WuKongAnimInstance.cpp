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

	UpdateAimingValues();

	// 更新移动数据
	if (MovementState == EWKMovementState::Grounded)
	{
		bShouldMove = ShouldMoveCheck();
		if (bShouldMove)
		{
			UpdateMovementValues();
			UpdateRotationValues();	
		}
	}
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

void UWuKongAnimInstance::UpdateAimingValues()
{
	// 控制器和角色旋转差
	FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(AimingRotation, OwnerWuKong->GetActorRotation());
	AimingAngle.X = DeltaRotator.Yaw;
	AimingAngle.Y = DeltaRotator.Pitch;
}

void UWuKongAnimInstance::UpdateMovementValues()
{
	VelocityBlend = InterpVelocityBlend(VelocityBlend, CalculateVelocityBlend(), VelocityBlendInterpSpeed, GetWorld()->GetDeltaSeconds());
	TurnBlend = CalculateTurnBlend();
	RunPlayRate = Speed / 600.f;
}

bool UWuKongAnimInstance::ShouldMoveCheck() const
{
	return (bIsMoving && bHasMovementInput) || Speed > 150.f;
}

void UWuKongAnimInstance::UpdateRotationValues()
{
	MovementDirection = CalculateMovementDirection();
	
	// 两种计算TurnAngle的方法等价
	FVector CurrentAcceleration = OwnerWuKong->GetCharacterMovement()->GetCurrentAcceleration();
	FVector LocalAcceleration = UKismetMathLibrary::LessLess_VectorRotator(CurrentAcceleration, OwnerWuKong->GetActorRotation());
	TurnAngle = UKismetMathLibrary::DegAtan2(LocalAcceleration.Y, LocalAcceleration.X);
	SmoothedTurnAngle = UKismetMathLibrary::Lerp(SmoothedTurnAngle, TurnAngle,  GetWorld()->GetDeltaSeconds() * SmoothedTurnAngleInterpSpeed);

	UpdateTurn180();
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

void UWuKongAnimInstance::OnTurnInPlaceComplete()
{
	bTurnInPlaceLeft = false;
	bTurnInPlaceRight = false;
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

void UWuKongAnimInstance::OnEnterLockTarget(AWKCharacterBase* LockableTarget)
{
	bHasLockTarget = true;

	// 检查与目标的角度差
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerWuKong->GetActorLocation(), LockableTarget->GetActorLocation());
	float RotationYaw = OwnerWuKong->GetActorRotation().Yaw;
	// 角度范围转换到[0,360]
	if (RotationYaw < 0)
	{
		RotationYaw += 360.f;
	}
	float LookAtRotationYaw = LookAtRotation.Yaw;
	if (LookAtRotationYaw < 0)
	{
		LookAtRotationYaw += 360.f;
	}
	TurnInPlaceYaw = FMath::Abs(RotationYaw - LookAtRotationYaw);
	// 触发旋转
	if (TurnInPlaceYaw >= 0.f && TurnInPlaceYaw < 180.f)
	{
		// 角色需要向右旋转
		bTurnInPlaceLeft = false;
		bTurnInPlaceRight = true;
	}
	else
	{
		// 角色需要向左旋转
		bTurnInPlaceLeft = true;
		bTurnInPlaceRight = false;
	}
}

void UWuKongAnimInstance::OnExitLockTarget()
{
	bHasLockTarget = false;

	bTurnInPlaceLeft = false;
	bTurnInPlaceRight = false;
}

float UWuKongAnimInstance::CalculateTurnBlend() const
{
	if (TurnAngle < 0.f)
	{
		return UKismetMathLibrary::MapRangeClamped(TurnAngle, -180.f, 0.f, 1.f, 0.f);
	}
	return UKismetMathLibrary::MapRangeClamped(TurnAngle, 0.f, 180.f, 0.f, 1.f);
}

FWKVelocityBlend UWuKongAnimInstance::CalculateVelocityBlend() const
{
	FRotator ActorRotation = OwnerWuKong->GetActorRotation();
	// 速度在角色空间下的方向
	FVector LocRelativeVelocityDir = ActorRotation.UnrotateVector(Velocity);
	float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) + FMath::Abs(LocRelativeVelocityDir.Z);
	FVector RelativeDirection = LocRelativeVelocityDir / Sum;

	FWKVelocityBlend Result;
	Result.F = FMath::Clamp(RelativeDirection.X, 0.f, 1.f);
	Result.B = FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.f, 0.f));
	Result.L = FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.f, 0.f));
	Result.R = FMath::Clamp(RelativeDirection.Y, 0.f, 1.f);

	return Result;
}

FWKVelocityBlend UWuKongAnimInstance::InterpVelocityBlend(const FWKVelocityBlend& Current, const FWKVelocityBlend& Target, float InterpSpeed, float DeltaTime)
{
	FWKVelocityBlend Result;
	Result.F = UKismetMathLibrary::FInterpTo(Current.F, Target.F, DeltaTime, InterpSpeed);
	Result.B = UKismetMathLibrary::FInterpTo(Current.B, Target.B, DeltaTime, InterpSpeed);
	Result.L = UKismetMathLibrary::FInterpTo(Current.L, Target.L, DeltaTime, InterpSpeed);
	Result.R = UKismetMathLibrary::FInterpTo(Current.R, Target.R, DeltaTime, InterpSpeed);
	return Result;
}

EWKMovementDirection UWuKongAnimInstance::CalculateMovementDirection() const
{
	// 以速度为导向的，只能是向前
	if (RotationMode == EWKRotationMode::VelocityDirection)
	{
		return EWKMovementDirection::Forward;
	}

	// 朝向目标的，计算角度区间倾向
	float AngleYaw = UKismetMathLibrary::NormalizedDeltaRotator(UKismetMathLibrary::MakeRotFromX(Velocity), AimingRotation).Yaw;
	return CalculateQuadrant(MovementDirection, MoveFRThreshold, MoveFLThreshold, MoveBRThreshold, MoveBLThreshold, MoveBuffer, AngleYaw);
}

EWKMovementDirection UWuKongAnimInstance::CalculateQuadrant(EWKMovementDirection Current, float FRThreshold, float FLThreshold, float BRThreshold, float BLThreshold, float Buffer, float Angle)
{
	if (AngleInRange(Angle, FLThreshold, FRThreshold, Buffer, Current != EWKMovementDirection::Forward || Current != EWKMovementDirection::Backward))
	{
		return EWKMovementDirection::Forward;
	}
	
	if (AngleInRange(Angle, FRThreshold, BRThreshold, Buffer, Current != EWKMovementDirection::Right || Current != EWKMovementDirection::Left))
	{
		return EWKMovementDirection::Right;
	}

	if (AngleInRange(Angle, BLThreshold, FLThreshold, Buffer, Current != EWKMovementDirection::Right || Current != EWKMovementDirection::Left))
	{
		return EWKMovementDirection::Left;
	}
	return EWKMovementDirection::Backward;
}

bool UWuKongAnimInstance::AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool bIncreaseBuffer)
{
	if (bIncreaseBuffer)
	{
		return UKismetMathLibrary::InRange_FloatFloat(Angle, MinAngle - Buffer, MaxAngle + Buffer);
	}
	return UKismetMathLibrary::InRange_FloatFloat(Angle, MinAngle + Buffer, MaxAngle - Buffer);
}
