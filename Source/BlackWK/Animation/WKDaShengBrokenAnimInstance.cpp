// Fill out your copyright notice in the Description page of Project Settings.


#include "WKDaShengBrokenAnimInstance.h"

#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/WKGameplayTags.h"
#include "BlackWK/Character/WKAIDaShengCharacter.h"

void UWKDaShengBrokenAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwnerWK.IsValid())
	{
		OwnerDaSheng = Cast<AWKAIDaShengCharacter>(OwnerWK);
	}
}

void UWKDaShengBrokenAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IWKAIStateInterface* PawnStateInterface = Cast<IWKAIStateInterface>(OwnerDaSheng))
	{
		bHasSawTarget = PawnStateInterface->HasSawEnemyTarget();
	}
}

void UWKDaShengBrokenAnimInstance::OnEnterStandIdle()
{
	bHasStandIdle = true;
	if (UWKAbilitySystemComponent* ASC = OwnerDaSheng->GetWKAbilitySystemComponent())
	{
		ASC->RemoveLooseGameplayTag(WKGameplayTags::Gameplay_State_Invincible);
	}
}

void UWKDaShengBrokenAnimInstance::OnEnterSitDownIdle()
{
}
