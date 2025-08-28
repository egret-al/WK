// Fill out your copyright notice in the Description page of Project Settings.


#include "WKImpactExecution.h"

#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/WKGameplayTags.h"
#include "BlackWK/AbilitySystem/AttributeSets/WKCombatSet.h"

struct WKImpactStatics
{
	WKImpactStatics()
	{
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UWKCombatSet, ImpactAmount, Source, true);
	}
	
	static const WKImpactStatics& GetDefault()
	{
		static WKImpactStatics ImpactStatics;
		return ImpactStatics;
	}
	
	// DECLARE_ATTRIBUTE_CAPTUREDEF(ImpactAmount);
};

UWKImpactExecution::UWKImpactExecution()
{
	const WKImpactStatics& ImpactDefault = WKImpactStatics::GetDefault();

	// RelevantAttributesToCapture.Add(ImpactDefault.ImpactAmountDef);
}

void UWKImpactExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const WKImpactStatics& ImpactDefault = WKImpactStatics::GetDefault();

	UWKAbilitySystemComponent* TargetASC = Cast<UWKAbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	UWKAbilitySystemComponent* SourceASC = Cast<UWKAbilitySystemComponent>(ExecutionParams.GetSourceAbilitySystemComponent());
	
	AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// 无敌、霸体状态下免疫冲击力
	if (TargetTags->HasTagExact(WKGameplayTags::Gameplay_State_Invincible) || TargetTags->HasTagExact(WKGameplayTags::Gameplay_State_IronBody))
	{
		return;
	}

	// 属性获取
	// float ImpactAmount = 0.f;
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ImpactDefault.ImpactAmountDef, EvaluateParameters, ImpactAmount);
}
