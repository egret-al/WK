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

void UWuKongAnimInstance::UpdateInput()
{
	MoveInput = OwnerWuKong->GetMoveInput();
}

void UWuKongAnimInstance::UpdateRotation()
{
	FTransform Transform = OwnerWuKong->GetActorTransform();
	// 获取加速度相对于角色空间下的末端位置
	// 将加速度转换到角色空间下
	FTransform AccelerationTransform;
	AccelerationTransform.SetLocation(OwnerWuKong->GetCharacterMovement()->GetCurrentAcceleration() + Transform.GetLocation());
	FTransform RelativeTransform = AccelerationTransform.GetRelativeTransform(Transform);
	TurnAngle = UKismetMathLibrary::DegAtan2(RelativeTransform.GetLocation().Y, RelativeTransform.GetLocation().X);

	// 两种计算TurnAngle的方法等价
	// FVector RelativeAccel = UKismetMathLibrary::LessLess_VectorRotator(Acceleration, OwnerWuKong->GetActorRotation());
	// TurnAngle = UKismetMathLibrary::DegAtan2(RelativeAccel.Y, RelativeAccel.X);

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
