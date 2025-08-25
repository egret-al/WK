// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

namespace WKCollision
{
	struct FCollisionShapeCache
	{
		FCollisionShape CollisionShape;
		FTransform RelativeTransform;

		FCollisionShapeCache(const FCollisionShape& InShape, const FTransform& InRelativeTrans)
			: CollisionShape(InShape)
			, RelativeTransform(InRelativeTrans)
		{};
	};

	struct FSkeletalMeshBoneCollisionDataCache
	{
		int32 BoneIndex;
		FTransform BoneTransform;
		TArray<FCollisionShapeCache> CollisionShapes;
		
		FSkeletalMeshBoneCollisionDataCache(int32 InBoneIdx, const FTransform& InBoneTrans, const TArray<FCollisionShapeCache>& InShapes)
			: BoneIndex(InBoneIdx)
			, BoneTransform(InBoneTrans)
			, CollisionShapes(InShapes)
		{};
	};

	TArray<FSkeletalMeshBoneCollisionDataCache> GetBoneCollisionsFromMesh(USkeletalMeshComponent* MeshComp, const FTransform& MeshComponentLocalToWorld, FName ShapeName = NAME_None);

	void DrawDebugBoneCollisionData(class UWorld* MyWorld, const FSkeletalMeshBoneCollisionDataCache& BoneCollisionData, const FColor& Color, float Duration, float LineThickness = 1.f);

}