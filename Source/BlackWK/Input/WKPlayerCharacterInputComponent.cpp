// Fill out your copyright notice in the Description page of Project Settings.


#include "WKPlayerCharacterInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "InputMappingContext.h"
#include "WKEnhancedInputComponent.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/AbilitySystem/WKGameplayTags.h"
#include "BlackWK/AbilitySystem/Abilities/WKAbilityTypes.h"
#include "BlackWK/Character/WKCharacterAnimInfoInterface.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/Character/WKPawnData.h"
#include "BlackWK/Player/WKLocalPlayer.h"
#include "BlackWK/Player/WKPlayerState.h"
#include "UserSettings/EnhancedInputUserSettings.h"

UWKPlayerCharacterInputComponent::UWKPlayerCharacterInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReadyToBindInputs = false;
	EnumPtr = StaticEnum<EWKGameplayAbilityInputBinds>();
}

UWKPlayerCharacterInputComponent* UWKPlayerCharacterInputComponent::FindPlayerCharacterInputComponent(const AActor* InActor)
{
	return InActor ? InActor->FindComponentByClass<UWKPlayerCharacterInputComponent>() : nullptr;
}

void UWKPlayerCharacterInputComponent::BeginPlay()
{
	Super::BeginPlay();

	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		InitializePlayerInput(OwnerPawn->GetController()->InputComponent);
	}
}

void UWKPlayerCharacterInputComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UWKPlayerCharacterInputComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		return;
	}

	// 获取输入子系统
	const APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	check(PC);

	const UWKLocalPlayer* LP = Cast<UWKLocalPlayer>(PC->GetLocalPlayer());
	check(LP);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	Subsystem->ClearAllMappings();

	AWKPlayerState* PS = PC->GetPlayerState<AWKPlayerState>();
	check(PS);

	const UWKPawnData* PawnData = PS->GetPawnData<UWKPawnData>();
	check(PawnData);

	for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
	{
		if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
		{
			if (Mapping.bRegisterWithSettings)
			{
				if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
				{
					Settings->RegisterInputMappingContext(IMC);
				}

				FModifyContextOptions Options = {};
				Options.bIgnoreAllPressedKeysUntilRelease = false;
				Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
			}
		}
	}
			
	// 注册输入
	UWKEnhancedInputComponent* EnhancedInputComponent = Cast<UWKEnhancedInputComponent>(PlayerInputComponent);
	if (ensureMsgf(EnhancedInputComponent, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UBDEnhancedInputComponent or a subclass of it.")))
	{
		EnhancedInputComponent->AddInputMappings(PawnData->InputConfig, Subsystem);

		TArray<uint32> BindHandles;
		EnhancedInputComponent->BindAbilityActions(PawnData->InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
		EnhancedInputComponent->BindNativeAction(PawnData->InputConfig, TEXT("Move"), ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
		EnhancedInputComponent->BindNativeAction(PawnData->InputConfig, TEXT("Look_Mouse"), ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
}

void UWKPlayerCharacterInputComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		Pawn->AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		Pawn->AddMovementInput(MovementDirection, Value.Y);
	}
}

void UWKPlayerCharacterInputComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = Cast<APawn>(GetOwner());

	if (!Pawn)
	{
		return;
	}
	if (IWKCharacterAnimInfoInterface* AnimInfoInterface = Cast<IWKCharacterAnimInfoInterface>(Pawn))
	{
		if (AnimInfoInterface->GetCharacterState().RotationMode == EWKRotationMode::LookingDirection)
		{
			return;
		}
	}
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UWKPlayerCharacterInputComponent::Input_AbilityInputTagPressed(FName InputName, bool bCheckLongPress)
{
	const AWKCharacterBase* CharacterBase = Cast<AWKCharacterBase>(GetOwner());
	if (IsValid(CharacterBase))
	{
		if (UWKAbilitySystemComponent* ASC = CharacterBase->GetWKAbilitySystemComponent())
		{
			const int32 InputID = GetInputEnumPtr()->GetIndexByName(InputName);
			ASC->AbilityLocalInputPressed(InputID);
		}
	}
}

void UWKPlayerCharacterInputComponent::Input_AbilityInputTagReleased(FName InputName)
{
	const AWKCharacterBase* CharacterBase = Cast<AWKCharacterBase>(GetOwner());
	if (IsValid(CharacterBase))
	{
		if (UWKAbilitySystemComponent* ASC = CharacterBase->GetWKAbilitySystemComponent())
		{
			const int32 InputID = GetInputEnumPtr()->GetIndexByName(InputName);
			ASC->AbilityLocalInputReleased(InputID);
		}
	}
}
