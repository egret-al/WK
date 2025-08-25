// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimationFunctionLibrary.h"

void UWKAnimationFunctionLibrary::DrawDebugSkeletalMeshPose(const USkeletalMeshComponent* MeshComp, EBoneSpaces::Type Space, float Duration, FColor Color)
{
	int32 BoneCount = MeshComp->GetNumBones();
	for (int32 BoneIndex = BoneCount; BoneIndex > 0; --BoneIndex)
	{
		FName BoneName = MeshComp->GetBoneName(BoneIndex);
		if (BoneName.IsNone())
		{
			continue;
		}
		FVector BoneLocation = MeshComp->GetBoneLocation(BoneName,Space);
		DrawDebugSphere(MeshComp->GetWorld(), BoneLocation, 1.0f, 12, Color, false, Duration, 0, 1.0f);
		
		FName ParentBoneName = MeshComp->GetParentBone(BoneName);
		int32 ParentBoneIndex = MeshComp->GetBoneIndex(ParentBoneName);
		if (ParentBoneIndex != INDEX_NONE)
		{
			FVector ParentBoneLocation = MeshComp->GetBoneLocation(ParentBoneName,Space);
			DrawDebugLine(MeshComp->GetWorld(), BoneLocation, ParentBoneLocation, Color, false, Duration, 0, 1.0f);
		}
	}
}
