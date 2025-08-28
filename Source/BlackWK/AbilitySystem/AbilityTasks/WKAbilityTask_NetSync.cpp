// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilityTask_NetSync.h"

#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"

void UWKAbilityTask_NetSync::OnDestroy(bool bInOwnerFinished)
{
	if (UWKAbilitySystemComponent* ASC = Cast<UWKAbilitySystemComponent>(AbilitySystemComponent.Get()))
	{
		// ASC->ClearAbilityReplicatedDataCacheEx(GetAbilitySpecHandle(), NetSyncGuid);
	}

	Super::OnDestroy(bInOwnerFinished);
}
