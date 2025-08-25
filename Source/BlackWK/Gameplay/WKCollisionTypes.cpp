// Fill out your copyright notice in the Description page of Project Settings.


#include "WKCollisionTypes.h"

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 5 
#include "PhysicsEngine/SkeletalBodySetup.h"
#endif // ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 5 

#include "DrawDebugHelpers.h"
#include "PhysicsEngine/PhysicsAsset.h"

TArray<WKCollision::FSkeletalMeshBoneCollisionDataCache> WKCollision::GetBoneCollisionsFromMesh(USkeletalMeshComponent* MeshComp, const FTransform& MeshComponentLocalToWorld, FName ShapeName /*= NAME_None*/)
{
	TArray<WKCollision::FSkeletalMeshBoneCollisionDataCache> Out;

	if (MeshComp && MeshComp->GetPhysicsAsset())
	{
		for (const USkeletalBodySetup* BS : MeshComp->GetPhysicsAsset()->SkeletalBodySetups)
		{
			const int32 BoneIndex = MeshComp->GetBoneIndex(BS->BoneName);
			if (BoneIndex != INDEX_NONE)
			{
				if (BS->AggGeom.GetElementCount() == 0)
				{
					UE_LOG(LogTemp, Error, TEXT("%s's AggGeom has no element"), *GetNameSafe(BS));
				}

				if (BS && BS->AggGeom.GetElementCount() > 0)
				{
					TArray<FCollisionShapeCache> Shapes;
					for (const FKBoxElem& Box : BS->AggGeom.BoxElems)
					{
						if (ShapeName == NAME_None || Box.GetName().ToString().Contains(ShapeName.ToString()))
						{
							FCollisionShape BoxShape;
							BoxShape.SetBox(FVector3f(Box.X/2, Box.Y / 2, Box.Z / 2) );
							Shapes.Add(FCollisionShapeCache(BoxShape, FTransform(Box.Rotation, Box.Center, FVector::OneVector)));
						}
					}
					for (const FKSphereElem& Sphere : BS->AggGeom.SphereElems)
					{
						if (ShapeName == NAME_None || Sphere.GetName().ToString().Contains(ShapeName.ToString()))
						{
							FCollisionShape SphereShape;
							SphereShape.SetSphere(Sphere.Radius);
							Shapes.Add(FCollisionShapeCache(SphereShape, FTransform(FQuat::Identity, Sphere.Center, FVector::OneVector)));
						}
					}
					for (const FKSphylElem& Sphyl : BS->AggGeom.SphylElems)
					{
						if (ShapeName == NAME_None || Sphyl.GetName().ToString().Contains(ShapeName.ToString()))
						{
							FCollisionShape SphylShape;
							SphylShape.SetCapsule(Sphyl.Radius, Sphyl.Length / 2 + Sphyl.Radius);
							Shapes.Add(FCollisionShapeCache(SphylShape, FTransform(Sphyl.Rotation, Sphyl.Center, FVector::OneVector)));
						}
					}

					if (Shapes.Num() > 0)
					{
						Out.Add(FSkeletalMeshBoneCollisionDataCache(BoneIndex, MeshComp->GetBoneTransform(BoneIndex, MeshComponentLocalToWorld), Shapes));
					}
				}
			}
		}
	}

	return Out;
}

void WKCollision::DrawDebugBoneCollisionData(class UWorld* MyWorld, const FSkeletalMeshBoneCollisionDataCache& BoneCollisionData, const FColor& Color, float Duration, float LineThickness /*= 1.f*/)
{
	if (MyWorld)
	{
		for (const FCollisionShapeCache& C : BoneCollisionData.CollisionShapes)
		{
			const FTransform CollisionTrans = C.RelativeTransform * BoneCollisionData.BoneTransform;
			if (C.CollisionShape.IsBox())
			{
				DrawDebugBox(MyWorld, CollisionTrans.GetTranslation(), C.CollisionShape.GetBox(), CollisionTrans.GetRotation(), Color, false, Duration, 0, LineThickness);
			}
			else if (C.CollisionShape.IsSphere())
			{
				DrawDebugSphere(MyWorld, CollisionTrans.GetTranslation(), C.CollisionShape.GetSphereRadius(), 24, Color, false, Duration, 0, LineThickness);
			}
			else if (C.CollisionShape.IsCapsule())
			{
				DrawDebugCapsule(MyWorld, CollisionTrans.GetTranslation(), C.CollisionShape.GetCapsuleHalfHeight(), C.CollisionShape.GetCapsuleRadius(), CollisionTrans.GetRotation(), Color, false, Duration, 0, LineThickness);
			}
		}
	}
}