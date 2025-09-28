// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayStatics.h"

#include "AbilitySystemInterface.h"
#include "BlackWK/AbilitySystem/WKAbilitySystemComponent.h"
#include "BlackWK/Animation/WKAnimationConfig.h"
#include "BlackWK/Animation/WKAnimationTypes.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/Character/WKPawnData.h"
#include "BlackWK/Player/WKPlayerController.h"
#include "Kismet/KismetMathLibrary.h"

bool UWKGameplayStatics::WorldIsGame(const UObject* WorldContextObj)
{
	if (WorldContextObj && WorldContextObj->GetWorld())
	{
		const EWorldType::Type WorldType = WorldContextObj->GetWorld()->WorldType;
		if (WorldType == EWorldType::PIE || WorldType == EWorldType::Game)
		{
			return true;
		}
	}

	return false;
}

UWKAbilitySystemComponent* UWKGameplayStatics::GetWKAbilitySystemComponent(AActor* InActor)
{
	if (!InActor)
	{
		return nullptr;
	}

	if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(InActor))
	{
		return Cast<UWKAbilitySystemComponent>(AbilityInterface->GetAbilitySystemComponent());
	}
	
	return nullptr;
}

APlayerController* UWKGameplayStatics::GetLocalPlayerController(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (PlayerController && PlayerController->GetLocalPlayer() && PlayerController->GetLocalPlayer()->IsA<ULocalPlayer>())
			{
				return PlayerController;
			}
		}
	}
	return nullptr;
}

TArray<AWKPlayerController*> UWKGameplayStatics::GetAllPlayerController(const UObject* WorldContextObject)
{
	TArray<AWKPlayerController*> Out;

	if (WorldContextObject->GetWorld())
	{
		for (FConstPlayerControllerIterator Iterator = WorldContextObject->GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (AWKPlayerController* PC = Cast<AWKPlayerController>(Iterator->Get()))
			{
				Out.Add(PC);
			}
		}
	}
	return Out;
}

bool UWKGameplayStatics::CalculateBeHitInfo(const AWKCharacterBase* InInstigator, const AWKCharacterBase* BeHitCharacter, const FHitResult& HitResult, float HitImpact, UAnimMontage*& OutHitMontage, FName& OutSectionName)
{
	if (!InInstigator || !BeHitCharacter)
	{
		return false;
	}

	// 获取角色上的动画配置
	const UWKPawnData* BeHitPawnData = BeHitCharacter->GetPawnData();
	if (!BeHitPawnData)
	{
		return false;
	}

	const UWKAnimationConfig* AnimationConfig = BeHitPawnData->AnimationConfig;
	if (!AnimationConfig)
	{
		return false;
	}

	// 计算受击方位
	const FVector VectorRotator = UKismetMathLibrary::LessLess_VectorRotator(HitResult.ImpactPoint - BeHitCharacter->GetActorLocation(), BeHitCharacter->GetActorRotation());
	float HitAngle = UKismetMathLibrary::DegAtan2(VectorRotator.Y, VectorRotator.X);

	// 检查一下配置文件中，是否存在左右、后的受击配置
	const FWKBeHitImpact& BeHitImpact = AnimationConfig->BeHitImpact;
	if (BeHitImpact.BeHitImpactDirections.IsEmpty() || BeHitImpact.BeHitMontage.IsNull())
	{
		return false;
	}
	
	OutHitMontage = BeHitImpact.BeHitMontage.LoadSynchronous();

	EWKAnimDirection BeHitDirection;
	const bool bHasBack = BeHitImpact.BeHitImpactDirections.Contains(EWKAnimDirection::B);
	// 如果背部受击都没有，认为只有正面受击，不考虑只配了正面和左右的情况
	if (!bHasBack)
	{
		BeHitDirection = EWKAnimDirection::F;
	}
	else
	{
		// 检查是否有配置左右受击
		if (BeHitImpact.BeHitImpactDirections.Contains(EWKAnimDirection::L) && BeHitImpact.BeHitImpactDirections.Contains(EWKAnimDirection::R))
		{
			// 考虑四向受击
			if (HitAngle >= -45.f && HitAngle <= 45.f)
			{
				BeHitDirection = EWKAnimDirection::F;
			}
			else if (HitAngle >= -135.f && HitAngle <= -45.f)
			{
				BeHitDirection = EWKAnimDirection::L;
			}
			else if (HitAngle >= 45.f && HitAngle <= 135.f)
			{
				BeHitDirection = EWKAnimDirection::R;
			}
			else
			{
				BeHitDirection = EWKAnimDirection::B;
			}
		}
		else
		{
			// 考虑两向受击
			if (HitAngle >= -90.f && HitAngle <= 90.f)
			{
				BeHitDirection = EWKAnimDirection::F;
			}
			else
			{
				BeHitDirection = EWKAnimDirection::B;
			}
		}
	}

	const FWKDirectionHitImpactAmount* FindBeHitImpactAmount = BeHitImpact.BeHitImpactDirections.Find(BeHitDirection);
	if (!FindBeHitImpactAmount || FindBeHitImpactAmount->BeHitImpacts.IsEmpty())
	{
		// 计算出的该方向没有配置受击动画信息
		return false;
	}

	// 根据受击程度来选择
	TArray<FWKDirectionHitImpact> DirectionHitImpacts = FindBeHitImpactAmount->BeHitImpacts;
	DirectionHitImpacts.StableSort([](const FWKDirectionHitImpact& A, const FWKDirectionHitImpact& B)
	{
		return A.HitImpact < B.HitImpact;
	});

	// 根据当前受击程度，找到一个低于当前受击程度的FWKDirectionHitImpact（受击动画变多后可以考虑二分查找优化）
	int32 FindIndex = INDEX_NONE;
	for (int32 i = 0; i < DirectionHitImpacts.Num(); i++)
	{
		if (DirectionHitImpacts[i].HitImpact == HitImpact)
		{
			FindIndex = i;
			break;
		}

		if (DirectionHitImpacts[i].HitImpact > HitImpact)
		{
			FindIndex = i - 1;
			break;
		}
	}

	if (FindIndex == INDEX_NONE)
	{
		FindIndex = DirectionHitImpacts.Num() - 1;
	}

	if (FindIndex < 0 || FindIndex >= DirectionHitImpacts.Num())
	{
		// 索引错误
		return false;
	}
	
	OutSectionName = DirectionHitImpacts[FindIndex].SectionNames[FMath::RandRange(0, DirectionHitImpacts[FindIndex].SectionNames.Num() - 1)];
	return true;
}
