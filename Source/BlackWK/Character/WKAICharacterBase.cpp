// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAICharacterBase.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AI/WKAIStateInterface.h"
#include "BlackWK/Player/WKPlayerState.h"
#include "BlackWK/UI/WKFloatingStatusBarWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

AWKAICharacterBase::AWKAICharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AWKAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	AWKPlayerState* PS = Cast<AWKPlayerState>(GetPlayerState());
	check(PS);
	AbilitySystemComponent = PS->GetWKAbilitySystemComponent();

	AbilitySystemComponent->InitAbilityActorInfo(GetPlayerState(), this);
	OnAbilitySystemInitialized();

	AAIController* AIController = Cast<AAIController>(GetController());
	check(AIController);
	BlackboardComponent = AIController->GetBlackboardComponent();
}

bool AWKAICharacterBase::IsPlayerCharacter()
{
	return false;
}

AWKCharacterBase* AWKAICharacterBase::GetCurrentAttackLockTarget() const
{
	if (!BlackboardComponent)
	{
		return nullptr;
	}

	UObject* CurrentTarget = BlackboardComponent->GetValueAsObject(FName(TEXT("CurrentTarget")));
	return Cast<AWKCharacterBase>(CurrentTarget);
}

bool AWKAICharacterBase::HasSawEnemyTarget()
{
	if (IWKAIStateInterface* StateController = Cast<IWKAIStateInterface>(GetController()))
	{
		return StateController->HasSawEnemyTarget();
	}
	return false;
}

FVector AWKAICharacterBase::GetLockLocation_Implementation()
{
	return GetActorLocation();
}
