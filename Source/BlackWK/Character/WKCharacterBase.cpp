// Fill out your copyright notice in the Description page of Project Settings.


#include "WKCharacterBase.h"

#include "WKCharacterMovementComponent.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/AttributeSets/WKAttributeSetBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

AWKCharacterBase::AWKCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UWKCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	bAlwaysRelevant = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
}

void AWKCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	OnBeginPlay();
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

UAbilitySystemComponent* AWKCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

UWKAbilitySystemComponent* AWKCharacterBase::GetWKAbilitySystemComponent() const
{
	return Cast<UWKAbilitySystemComponent>(GetAbilitySystemComponent());
}

float AWKCharacterBase::GetHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float AWKCharacterBase::GetMaxHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float AWKCharacterBase::GetMana() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMana();
	}

	return 0.0f;
}

float AWKCharacterBase::GetMaxMana() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxMana();
	}

	return 0.0f;
}

float AWKCharacterBase::GetStamina() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetStamina();
	}

	return 0.0f;
}

float AWKCharacterBase::GetMaxStamina() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxStamina();
	}

	return 0.0f;
}

float AWKCharacterBase::GetHulu() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetHulu();
	}

	return 0.0f;
}

float AWKCharacterBase::GetMaxHulu() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxHulu();
	}

	return 0.0f;
}

float AWKCharacterBase::GetMoveSpeed() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeed();
	}

	return 0.0f;
}

float AWKCharacterBase::GetMoveSpeedBaseValue() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBase.Get())->GetBaseValue();
	}

	return 0.0f;
}

void AWKCharacterBase::SetHealth(float Health)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetHealth(Health);
	}
}

void AWKCharacterBase::SetMana(float Mana)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetMana(Mana);
	}
}

void AWKCharacterBase::SetStamina(float Stamina)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetStamina(Stamina);
	}
}

void AWKCharacterBase::SetHulu(float Hulu)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetHulu(Hulu);
	}
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
