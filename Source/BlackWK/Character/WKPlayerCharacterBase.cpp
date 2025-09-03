// Fill out your copyright notice in the Description page of Project Settings.


#include "WKPlayerCharacterBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "WKAICharacterBase.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/Player/WKPlayerController.h"
#include "BlackWK/Player/WKPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BlackWK/AbilitySystem/AttributeSets/WKAttributeSetBase.h"
#include "BlackWK/Animation/WKAnimInstanceExtensionInterface.h"
#include "BlackWK/Input/WKPlayerCharacterInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AWKPlayerCharacterBase::AWKPlayerCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation(FVector(0, 0, 68.492264));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->FieldOfView = 80.0f;

	// GunComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Gun"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// Makes sure that the animations play on the Server so that we can use bone and socket transforms
	// to do things like spawning projectiles and other FX.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	bUseControllerRotationYaw = false;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void AWKPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
	StartingCameraBoomLocation = CameraBoom->GetRelativeLocation();

	if (GetNetMode() == NM_Standalone)
	{
		InitializeAbilityClient();
	}
}

void AWKPlayerCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AWKPlayerCharacterBase::Input_Lock(const FInputActionValue& InputActionValue)
{
	if (IsValid(LockTarget))
	{
		// 清除锁定
		LockTarget = nullptr;
		LockTargetSnapshotControlRotation = FRotator();
		LockStartRotation = LockTargetSnapshotControlRotation;
		LockInterpTime = 0.0f;
		bLockInterpInProgress = false;

		SetRotationMode(EWKRotationMode::VelocityDirection);

		if (IWKAnimInstanceExtensionInterface* AnimInstanceExtension = Cast<IWKAnimInstanceExtensionInterface>(GetMesh()->GetAnimInstance()))
		{
			AnimInstanceExtension->OnExitLockTarget();
		}
	}
	else
	{
		// 查找AI
		TArray<AActor*> FindActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWKAICharacterBase::StaticClass(), FindActors);
		if (!FindActors.IsEmpty())
		{
			LockTarget = Cast<AWKCharacterBase>(FindActors[0]);
		}

		if (IsValid(LockTarget))
		{
			LockTargetSnapshotControlRotation = GetControlRotation();
			LockStartRotation = LockTargetSnapshotControlRotation;
			bLockInterpInProgress = true;
			
			SetRotationMode(EWKRotationMode::LookingDirection);
			if (IWKAnimInstanceExtensionInterface* AnimInstanceExtension = Cast<IWKAnimInstanceExtensionInterface>(GetMesh()->GetAnimInstance()))
			{
				AnimInstanceExtension->OnEnterLockTarget(LockTarget);
			}
		}
	}
}

void AWKPlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (AWKPlayerController* PC = Cast<AWKPlayerController>(GetController()))
	{
		ULocalPlayer* LP = PC->GetLocalPlayer();
		check(LP);

		UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		check(InputLocalPlayerSubsystem);

		InputLocalPlayerSubsystem->ClearAllMappings();
		InputLocalPlayerSubsystem->AddMappingContext(IMC_Default, 0);

		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		check(EnhancedInputComponent);
		EnhancedInputComponent->BindAction(IA_Lock, ETriggerEvent::Started, this, &ThisClass::Input_Lock);
	}
}

void AWKPlayerCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateLockTargetCameraLocation();
}

void AWKPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	AWKPlayerState* PS = GetPlayerState<AWKPlayerState>();
	if (PS)
	{
		AbilitySystemComponent = Cast<UWKAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		OnAbilitySystemInitialized();
	}
}

void AWKPlayerCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeAbilityClient();
}

void AWKPlayerCharacterBase::InitializeAbilityClient()
{
	if (UWKPlayerCharacterInputComponent* WKInputComponent = UWKPlayerCharacterInputComponent::FindPlayerCharacterInputComponent(this))
	{
		WKInputComponent->InitializePlayerInput(GetController()->InputComponent);
	}

	AWKPlayerState* PS = GetPlayerState<AWKPlayerState>();
	if (PS)
	{
		AbilitySystemComponent = Cast<UWKAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	}

	AWKPlayerController* PC = Cast<AWKPlayerController>(GetController());
	if (PC)
	{
		PC->CreateHUD();
	}
}

USpringArmComponent* AWKPlayerCharacterBase::GetCameraBoom() const
{
	return CameraBoom;
}

UCameraComponent* AWKPlayerCharacterBase::GetFollowCamera() const
{
	return FollowCamera;
}

float AWKPlayerCharacterBase::GetStartingCameraBoomArmLength()
{
	return StartingCameraBoomArmLength;
}

FVector AWKPlayerCharacterBase::GetStartingCameraBoomLocation()
{
	return StartingCameraBoomLocation;
}

bool AWKPlayerCharacterBase::IsPlayerCharacter()
{
	return true;
}

AWKCharacterBase* AWKPlayerCharacterBase::GetCurrentAttackLockTarget() const
{
	return LockTarget;
}

FVector2d AWKPlayerCharacterBase::GetMoveInput() const
{
	return UWKPlayerCharacterInputComponent::FindPlayerCharacterInputComponent(this)->MoveInput;
}

AWKPlayerController* AWKPlayerCharacterBase::GetPlayerController() const
{
	return Cast<AWKPlayerController>(GetController());
}

void AWKPlayerCharacterBase::UpdateLockTargetCameraLocation()
{
	if (RotationMode == EWKRotationMode::LookingDirection)
	{
		if (!IsValid(LockTarget))
		{
			// 如果没有目标，切换回去
			SetRotationMode(EWKRotationMode::VelocityDirection);
		}
		else
		{
			// 目标方向
			FVector LockLocation = IWKLockableInterface::Execute_GetLockLocation(LockTarget);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockLocation);

			if (bLockInterpInProgress)
			{
				LockInterpTime += GetWorld()->GetDeltaSeconds();
				float Alpha = FMath::Clamp(LockInterpTime / LockInterpDuration, 0.0f, 1.0f);

				// 从曲线获取插值值
				float CurveAlpha = LockInterpCurve ? LockInterpCurve->GetFloatValue(Alpha) : Alpha;

				FRotator InterpRotation = FMath::Lerp(LockStartRotation, LookAtRotation, CurveAlpha);
				GetController()->SetControlRotation(InterpRotation);

				if (Alpha >= 1.0f)
				{
					bLockInterpInProgress = false;
				}
			}
			else
			{
				// 过渡完成后，加快插值速度
				FRotator InterpRotation = FMath::Lerp(GetController()->GetControlRotation(), LookAtRotation, GetWorld()->GetDeltaSeconds() * LockInterpSpeed);
				GetController()->SetControlRotation(InterpRotation);
			}
		}
	}
}

void AWKPlayerCharacterBase::SetRotationMode(EWKRotationMode NewRotationMode)
{
	if (NewRotationMode != RotationMode)
	{
		OnRotationModeChanged(NewRotationMode);
	}
}

void AWKPlayerCharacterBase::OnRotationModeChanged(EWKRotationMode NewRotationMode)
{
	RotationMode = NewRotationMode;
}
