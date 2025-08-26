// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WKGameDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWK_API UWKGameDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** 唯一的ID */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AssetRegistrySearchable, Category = "WKGameData")
	int32 DataID;

	/** 数据的类型 */
	UPROPERTY(BlueprintReadOnly, Category = "WKGameData")
	FPrimaryAssetType DataType;
};
