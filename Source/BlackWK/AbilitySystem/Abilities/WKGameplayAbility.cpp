// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/Character/WKCharacterBase.h"

UWKGameplayAbility::UWKGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UWKGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& AbilitySpec) const
{
	const bool bIsPredicting = GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Predicting;

	if (ActorInfo && !AbilitySpec.IsActive() && !bIsPredicting && ActivationPolicy == EWKAbilityActivationPolicy::OnSpawn)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// 如果Avatar死亡，不再激活
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && AvatarActor->GetLifeSpan() <= 0.f)
		{
			// 本地是否执行
			const bool bIsLocalExecution = NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted || NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly;
			// 服务端是否执行
			const bool bIsServerExecution = NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly || NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated;

			// 本地客户端是否应该激活
			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			// 服务端是否应该激活
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

AWKCharacterBase* UWKGameplayAbility::GetWKCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AWKCharacterBase>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

UWKAbilitySystemComponent* UWKGameplayAbility::GetWKAbilitySystemComponentFromActorInfo() const
{
	return Cast<UWKAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

void UWKGameplayAbility::AddGameplayTag(FGameplayTag AddToTag)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(AddToTag);
	}
}

void UWKGameplayAbility::RemoveGameplayTag(FGameplayTag RemoveToTag)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(RemoveToTag);
	}
}

void UWKGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	K2_OnAbilityAdded();
	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UWKGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	K2_OnAbilityRemoved();
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UWKGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	K2_InputPressed();
}

void UWKGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	K2_InputReleased();
}

void UWKGameplayAbility::OnPawnAvatarSet()
{
	K2_OnPawnAvatarSet();
}
