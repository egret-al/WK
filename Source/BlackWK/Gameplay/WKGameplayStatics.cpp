// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayStatics.h"

#include "AbilitySystemInterface.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"

bool UWKGameplayStatics::WorldIsGame(const UObject* WorldContextObj)
{
	if (WorldContextObj && WorldContextObj->GetWorld())
	{
		const EWorldType::Type WorldType = WorldContextObj->GetWorld()->WorldType;
		if (WorldType == EWorldType::PIE || WorldType == EWorldType::Game)
		{
			return true;
		}
	}

	return false;
}

UWKAbilitySystemComponent* UWKGameplayStatics::GetWKAbilitySystemComponent(AActor* InActor)
{
	if (!InActor)
	{
		return nullptr;
	}

	if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(InActor))
	{
		return Cast<UWKAbilitySystemComponent>(AbilityInterface->GetAbilitySystemComponent());
	}
	
	return nullptr;
}

float UWKGameplayStatics::CalculateDirection(const FVector& Direction, const FVector& FacingDirection)
{
	// 只在水平面上计算，忽略Z
	FVector Dir2D = FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal();
	FVector Facing2D = FVector(FacingDirection.X, FacingDirection.Y, 0.f).GetSafeNormal();

	if (Dir2D.IsNearlyZero() || Facing2D.IsNearlyZero())
	{
		return 0.f;
	}

	// atan2( cross, dot ) 得到 [-PI, PI]，再转角度
	float AngleRad = FMath::Atan2(
		FVector::CrossProduct(Facing2D, Dir2D).Z,   // 左右性
		FVector::DotProduct(Facing2D, Dir2D)        // 大小
	);

	return FMath::RadiansToDegrees(AngleRad); // 输出 [-180, 180]
}
