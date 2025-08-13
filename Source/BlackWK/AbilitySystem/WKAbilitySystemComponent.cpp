// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilitySystemComponent.h"

void UWKAbilitySystemComponent::ReceiveDamage(UWKAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
