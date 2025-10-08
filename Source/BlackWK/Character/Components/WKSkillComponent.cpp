// Fill out your copyright notice in the Description page of Project Settings.


#include "WKSkillComponent.h"

#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/DataAssets/WKDataTypes.h"

UWKSkillComponent::UWKSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWKSkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWKSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FDateTime Now = FDateTime::UtcNow();
	TArray<int32> DeletedSkills;
	for (auto SkillCooldown : SkillCooldownMap)
	{
		if (Now >= SkillCooldown.Value)
		{
			// CD到了
			DeletedSkills.Add(SkillCooldown.Key);
		}
	}

	for (const int32 SkillID : DeletedSkills)
	{
		SkillCooldownMap.Remove(SkillID);
	}
}

UAbilitySystemComponent* UWKSkillComponent::GetAbilitySystemComponent() const
{
	if (IAbilitySystemInterface* OwnerInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		return OwnerInterface->GetAbilitySystemComponent();
	}
	
	return nullptr;
}

UWKAbilitySystemComponent* UWKSkillComponent::GetWKAbilitySystemComponent() const
{
	return Cast<UWKAbilitySystemComponent>(GetAbilitySystemComponent());
}

bool UWKSkillComponent::IsSkillInCooldown(const int32 SkillID) const
{
	return SkillCooldownMap.Contains(SkillID);
}

bool UWKSkillComponent::GetSkillCooldown(const int32 SkillID, FDateTime& OutCooldown) const
{
	if (const FDateTime* FindTime = SkillCooldownMap.Find(SkillID))
	{
		OutCooldown = *FindTime;
		return true;
	}

	return false;
}

void UWKSkillComponent::StartSkillCooldown(const int32 SkillID)
{
	if (const UWKAbilitySystemComponent* ASC = GetWKAbilitySystemComponent())
	{
		if (FWKSkillTableRow SkillTableRow;
			ASC->GetSkillRowByID(SkillID, SkillTableRow))
		{
			float InitCooldown = SkillTableRow.InitCooldown;
			if (InitCooldown > 0)
			{
				// 加入CD计时
				FDateTime CooldownTime = FDateTime::UtcNow() + FTimespan(0.f, 0.f, InitCooldown);
				SkillCooldownMap.Add(SkillID, CooldownTime);
			}
		}
	}
}

void UWKSkillComponent::ClearSkillCooldown(const int32 SkillID)
{
	SkillCooldownMap.Remove(SkillID);
}

