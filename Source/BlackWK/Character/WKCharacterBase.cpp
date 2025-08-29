// Fill out your copyright notice in the Description page of Project Settings.


#include "WKCharacterBase.h"

#include "WKCharacterMovementComponent.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/AttributeSets/WKAttributeSetBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/WKHealthComponent.h"
#include "Components/WKSkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

AWKCharacterBase::AWKCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass<UWKCharacterMovementComponent>(CharacterMovementComponentName)
		.SetDefaultSubobjectClass<UWKSkeletalMeshComponent>(ACharacter::MeshComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	bAlwaysRelevant = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);

	HealthComponent = CreateDefaultSubobject<UWKHealthComponent>(TEXT("HealthComponent"));
}

void AWKCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	OnBeginPlay();
}

void AWKCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SimulatedOnlyPushModelBased{ COND_SimulatedOnly, REPNOTIFY_Always, true };
	DOREPLIFETIME_WITH_PARAMS_FAST(AWKCharacterBase, CurrentMeleeComboIndex, SimulatedOnlyPushModelBased);
}

void AWKCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetEssentialValues();
	
	CacheValues();
}

void AWKCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

FWKEssentialValue AWKCharacterBase::GetEssentialValues()
{
	FWKEssentialValue EssentialValue;
	EssentialValue.Velocity = GetVelocity();
	EssentialValue.Acceleration = Acceleration;
	EssentialValue.MovementInput = GetCharacterMovement()->GetCurrentAcceleration();
	EssentialValue.Speed = Speed;
	EssentialValue.MovementInputAmount = MovementInputAmount;
	EssentialValue.AimingRotation = GetControlRotation();
	return EssentialValue;
}

void AWKCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.Reset();
	if (GetWKAbilitySystemComponent())
	{
		GetWKAbilitySystemComponent()->GetOwnedGameplayTags(TagContainer);
	}
}

bool AWKCharacterBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (GetWKAbilitySystemComponent())
	{
		return GetWKAbilitySystemComponent()->HasMatchingGameplayTagExactly(TagToCheck);
	}
	return false;
}

bool AWKCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (GetWKAbilitySystemComponent())
	{
		return GetWKAbilitySystemComponent()->HasAnyMatchingGameplayTags(TagContainer);
	}
	return false;
}

bool AWKCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (GetWKAbilitySystemComponent())
	{
		return GetWKAbilitySystemComponent()->HasAllMatchingGameplayTags(TagContainer);
	}
	return false;
}

UAbilitySystemComponent* AWKCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

UWKAbilitySystemComponent* AWKCharacterBase::GetWKAbilitySystemComponent() const
{
	return Cast<UWKAbilitySystemComponent>(GetAbilitySystemComponent());
}

void AWKCharacterBase::PlayHitMontage_Implementation(AWKCharacterBase* InInstigator)
{
}

void AWKCharacterBase::ClearMeleeComboIndex()
{
	CurrentMeleeComboIndex = INDEX_NONE;
	MARK_PROPERTY_DIRTY_FROM_NAME(AWKCharacterBase, CurrentMeleeComboIndex, this);
}

void AWKCharacterBase::SetMeleeComboIndex(int32 Index)
{
	CurrentMeleeComboIndex = Index;
	MARK_PROPERTY_DIRTY_FROM_NAME(AWKCharacterBase, CurrentMeleeComboIndex, this);
}

void AWKCharacterBase::OnAbilitySystemInitialized()
{
	UWKAbilitySystemComponent* ASC = GetWKAbilitySystemComponent();
	check(ASC);

	HealthComponent->InitializeWithAbilitySystem(ASC);
}

void AWKCharacterBase::OnBeginPlay()
{
	TargetRotation = GetActorRotation();
	LastVelocityRotation = GetActorRotation();
	LastMovementInputRotation = GetActorRotation();
	
}

void AWKCharacterBase::SetEssentialValues()
{
	Acceleration = CalculateAcceleration();

	FVector Velocity = GetVelocity();
	Speed = Velocity.Size2D();

	bIsMoving = Speed > 1.f;
	if (bIsMoving)
	{
		LastVelocityRotation = UKismetMathLibrary::MakeRotFromX(GetVelocity());
	}

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementInputAmount = MovementComponent->GetCurrentAcceleration().Length() / MovementComponent->GetMaxAcceleration();
	bHasMovementInput = MovementInputAmount > 0.f;
	if (bHasMovementInput)
	{
		LastMovementInputRotation = UKismetMathLibrary::MakeRotFromX(MovementComponent->GetCurrentAcceleration());
	}

	FRotator ControlRotation = GetControlRotation();
	AimYawRate = FMath::Abs((ControlRotation.Yaw - PreviousAimYaw) / GetWorld()->GetDeltaSeconds());
}

void AWKCharacterBase::CacheValues()
{
	PreviousVelocity = GetVelocity();
	PreviousAimYaw = GetControlRotation().Yaw;
}

FVector AWKCharacterBase::CalculateAcceleration() const
{
	return (GetVelocity() - PreviousVelocity) / GetWorld()->GetDeltaSeconds();
}
