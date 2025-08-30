// Fill out your copyright notice in the Description page of Project Settings.


#include "WKDaShengBrokenAnimInstance.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/WKGameplayTags.h"
#include "BlackWK/AI/WKDaShengBrokenAIController.h"
#include "BlackWK/Character/WKAIDaShengCharacter.h"
#include "BlackWK/Gameplay/WKGameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UWKDaShengBrokenAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (!UWKGameplayStatics::WorldIsGame(GetWorld()))
	{
		return;
	}
	
	if (OwnerWK.IsValid())
	{
		OwnerDaSheng = Cast<AWKAIDaShengCharacter>(OwnerWK);
		InitInstanceInfo();
	}
}

void UWKDaShengBrokenAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!UWKGameplayStatics::WorldIsGame(GetWorld()))
	{
		return;
	}

	if (!OwnerController || !BlackboardComponent)
	{
		InitInstanceInfo();

		if (!OwnerController || !BlackboardComponent)
		{
			return;
		}
	}
	
	UpdateState();
	UpdateMovement();
}

void UWKDaShengBrokenAnimInstance::OnEnterStandIdle()
{
	bHasStandIdle = true;
	if (UWKAbilitySystemComponent* ASC = OwnerDaSheng->GetWKAbilitySystemComponent())
	{
		ASC->RemoveLooseGameplayTag(WKGameplayTags::Gameplay_State_Invincible);

		BlackboardComponent->SetValueAsBool(FName(TEXT("HasStand")), true);
	}
}

void UWKDaShengBrokenAnimInstance::OnEnterSitDownIdle()
{
}

void UWKDaShengBrokenAnimInstance::UpdateState()
{
	if (IWKAIStateInterface* PawnStateInterface = Cast<IWKAIStateInterface>(OwnerDaSheng))
	{
		bHasSawTarget = PawnStateInterface->HasSawEnemyTarget();
		CurrentTarget = PawnStateInterface->GetCurrentTarget();
	}
}

void UWKDaShengBrokenAnimInstance::UpdateTreeInfo()
{
	if (!BlackboardComponent)
	{
		return;
	}

	// 跟随目标
	bFollowTarget = BlackboardComponent->GetValueAsBool("FollowTarget");
}

void UWKDaShengBrokenAnimInstance::UpdateMovement()
{
	if (!CurrentTarget.IsValid())
	{
		TurnAngle = 0.f;
		return;
	}

	FVector Direction = CurrentTarget->GetActorLocation() - OwnerDaSheng->GetActorLocation();
	FVector LocalDirection = UKismetMathLibrary::LessLess_VectorRotator(Direction, OwnerDaSheng->GetActorRotation());
	TurnAngle = UKismetMathLibrary::DegAtan2(LocalDirection.Y, LocalDirection.X);
}

void UWKDaShengBrokenAnimInstance::InitInstanceInfo()
{
	OwnerController = Cast<AWKDaShengBrokenAIController>(OwnerDaSheng->GetController());
	if (OwnerController)
	{
		BlackboardComponent = OwnerController->GetBlackboardComponent();
	}
}
