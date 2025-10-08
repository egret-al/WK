// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WKDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FWKSkillTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	bool IsValid() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillID = INDEX_NONE;

	// 激活用到的Tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SkillTag;

	// 释放技能的技能范围
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D ReleaseDistCondition;

	// 技能CD
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitCooldown;

	// 技能描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Desc;
};
