// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotifyState_MeleeAttackDetection.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BlackWK/AbilitySystem/WKGameplayTags.h"
#include "BlackWK/AbilitySystem/Targeting/WKTargetData.h"
#include "BlackWK/Animation/Weapon/WKWeaponTypes.h"
#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/Character/Components/WKSkeletalMeshComponent.h"
#include "BlackWK/Gameplay/WKGameplayStatics.h"
#include "BlackWK/Player/WKPlayerState.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"

#define SHAPE_NAME_TRACE FName("TRACE")

template <class T, uint32 NumElements>
static void GetCandidateComponents(UObject& Obj, TInlineComponentArray<T*, NumElements>& OutComps)
{
	// No actor owner in some editor windows. Get SkeletalMeshComponent spawned by the MeshComp.
	ForEachObjectWithOuter(&Obj, [&OutComps](UObject* Child)
	{
		if (T* ChildComp = Cast<T>(Child))
		{
			OutComps.Add(ChildComp);
		}
	}, false, RF_NoFlags, EInternalObjectFlags::Garbage);
}

template <class TargetType, class T, uint32 NumElements>
static TargetType* FindAndRemoveFrom(TInlineComponentArray<T*, NumElements>& Components)
{
	TargetType* Out = nullptr;
	int32 Index = Components.IndexOfByPredicate([](const T* C)
	{
		return C != nullptr && C->IsA(TargetType::StaticClass());
	});
	if (Index != INDEX_NONE)
	{
		Out = Cast<TargetType>(Components[Index]);
		Components.RemoveAt(Index);
	}

	return Out;
}

void UWKAnimNotifyState_MeleeAttackDetection::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp == nullptr
		|| MeshComp->GetWorld() == nullptr
		|| MeshComp->GetOwner() == nullptr
		|| (UWKGameplayStatics::WorldIsGame(MeshComp) && MeshComp->GetOwner()->GetLocalRole() <= ROLE_SimulatedProxy))
	{
		return;
	}

	// 重置数据
	*HaveSendBeginCheckEventTagContainer.GetOrAdd(MeshComp) = false;

	if (UWKSkeletalMeshComponent* WKSkeletalMeshComp = Cast<UWKSkeletalMeshComponent>(MeshComp))
	{
		WKSkeletalMeshComp->bEnableUpdateRateOptimizations = false;
		WKSkeletalMeshComp->SetServerUpdateBoneTransforms(true);
	}

#if WITH_EDITORONLY_DATA
	OwnerMeshComp = MeshComp;
#endif

	WeaponMeshComp = nullptr;
	SetWeaponMeshComp(MeshComp, WeaponMeshComp);

	TraceShapeData.Empty();

	AActor* OwnerActor = MeshComp->GetOwner();
	if (OwnerActor)
	{
		FANSHitActorsData* HitActorsData = DataContainer.GetOrAdd(MeshComp);
		HitActorsData->AnimNotifyWeaponAttack_HitActors.Empty();
		
		//目前近战攻击需要空手，但瞄准时会装备武器，导致获取到武器Mesh，碰撞计算错误!
		if (bUseCustomDefinedCollision)
		{
			WeaponMeshComp = nullptr;
		}
		else
		{
			if (IWKCharacterAnimInfoInterface* AnimInfoInterface = Cast<IWKCharacterAnimInfoInterface>(OwnerActor))
			{
				WeaponMeshComp = AnimInfoInterface->GetWeaponMeshComponent();
			}
		}
		SetWeaponMeshComp(MeshComp, WeaponMeshComp);
	}

#if WITH_EDITOR
	// 创建编辑器窗口下的预览武器 mesh
	if (!UWKGameplayStatics::WorldIsGame(MeshComp))
	{
		if (WeaponMeshComp == nullptr)
		{
			TInlineComponentArray<USkeletalMeshComponent*, 2> SkeletalMeshComp;
			GetCandidateComponents(*MeshComp, SkeletalMeshComp);

			if (WeaponPreviewMesh)
			{
				if (SkeletalMeshComp.Num() > 0)
				{
					WeaponMeshComp = SkeletalMeshComp[0];
				}
				else
				{
					WeaponMeshComp = NewObject<USkeletalMeshComponent>(MeshComp);
				}

				if (WeaponMeshComp)
				{
					if (WeaponMeshComp->IsRegistered() == false)
					{
						WeaponMeshComp->RegisterComponentWithWorld(MeshComp->GetWorld());
					}

					//const FName WeaponSocketName = bLeftHandWeapon ? DSCharacterSkeletonNames::LeftWeaponSocket : DSCharacterSkeletonNames::RightWeaponSocket;
					WeaponMeshComp->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, PreviewMeshSocketName);
					WeaponMeshComp->SetSkeletalMesh(WeaponPreviewMesh);
					WeaponMeshComp->SetRelativeLocationAndRotation(PreviewMeshLocationOffset, PreviewMeshRotationOffset);
				}
			}
		}
	}
	
	if (!UWKGameplayStatics::WorldIsGame(MeshComp))
	{
		if (bUseCustomDefinedCollision)
		{
			// 为自定义碰撞创建编辑器预览的 ShapeComponent
			CreateDebugShapesForCustomCollision(OwnerMeshComp, CustomCollisionDefs);
		}
		else
		{
			CreateDebugShapesForCollisionData(OwnerMeshComp, TraceShapeData);
		}
	}
#endif


	// get CollisionShape data from weapon mesh's PhA
	if (WeaponMeshComp && !bUseCustomDefinedCollision)
	{
		TraceShapeData = WKCollision::GetBoneCollisionsFromMesh(WeaponMeshComp, WeaponMeshComp->GetComponentTransform(), SHAPE_NAME_TRACE);
		// if (TraceShapeData.Num() == 0)
		// {
		// 	DSLog(DSAnimNotifyState, Error, TEXT("NotifyBegin(%s): TraceShapeData is empty!"), *DSStringConversionHelper::NetRoleToString(MeshComp->GetOwner()->GetLocalRole()));
		// }
	}

	// 计算自定义碰撞的初始位置
	if (bUseCustomDefinedCollision)
	{
		const int32 NumCustomCollisionShapes = CustomCollisionDefs.Num();
		CustomCollisionShapes.SetNum(NumCustomCollisionShapes);

		TArray<FTransform>& LastCustomShapeTransform = GetLastCustomShapeTransform(MeshComp);
		LastCustomShapeTransform.SetNum(NumCustomCollisionShapes);
		for (int i = 0; i < NumCustomCollisionShapes; i++)
		{
			const FWeaponCustomCollisionDef& Def = CustomCollisionDefs[i];
			FTransform LocalTrans = WeaponMeshComp ? WeaponMeshComp->GetSocketTransform(Def.SocketName, RTS_Component) : MeshComp->GetSocketTransform(Def.SocketName, RTS_Component);
			LocalTrans = FTransform(Def.CollisionRotationOffset, Def.CollisionLocationOffset, FVector::OneVector) * LocalTrans;

			const FTransform Local2World = WeaponMeshComp ? WeaponMeshComp->GetComponentTransform() : MeshComp->GetComponentTransform();

			// 换算到世界坐标系
			LastCustomShapeTransform[i] = LocalTrans * Local2World;

			switch (Def.CollisionType)
			{
			case EWeaponCollisionType::Capsule:
				CustomCollisionShapes[i].SetCapsule(FMath::Max(Def.CollisionExtent.X, Def.CollisionExtent.Y), Def.CollisionExtent.Z / 2);
				break;

			case EWeaponCollisionType::Sphere:
				CustomCollisionShapes[i].SetSphere(FMath::Max3(Def.CollisionExtent.X, Def.CollisionExtent.Y, Def.CollisionExtent.Z));
				break;

			default:
				const FVector halfExtend = Def.CollisionExtent / 2;
				FVector3f boxVec;
				boxVec.X = halfExtend.X;
				boxVec.Y = halfExtend.Y;
				boxVec.Z = halfExtend.Z;
				CustomCollisionShapes[i].SetBox(boxVec);
				break;
			}
		}
	}
}

void UWKAnimNotifyState_MeleeAttackDetection::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp == nullptr
		|| MeshComp->GetWorld() == nullptr
		|| MeshComp->GetOwner() == nullptr
		|| (UWKGameplayStatics::WorldIsGame(MeshComp) && MeshComp->GetOwner()->GetLocalRole() <= ROLE_SimulatedProxy))
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	AWKCharacterBase* OwnerCharacter = Cast<AWKCharacterBase>(OwnerActor);

	if (MeshComp->GetWorld()->GetNetMode() != ENetMode::NM_Standalone)
	{
		if (OwnerCharacter)
		{
			//玩家的攻击由主控端判定
			if (OwnerCharacter->IsPlayerControlled())
			{
				AWKPlayerState* PlayerState = Cast<AWKPlayerState>(OwnerCharacter->GetPlayerState());
			}
			//怪物的攻击由服务器判定
			else
			{
				if (OwnerActor->GetLocalRole() != ROLE_Authority)
				{
					return;
				}
			}
		}
	}
	

	if (!bUseTickCheck)
	{
		return;
	}

	if(!OwnerCharacter)
	{
		return;
	}

	TArray<FTransform>& LastCustomShapeTransform = GetLastCustomShapeTransform(MeshComp);

	FANSHitActorsData* HitActorsDataData = DataContainer.GetOrAdd(MeshComp);
	HitActorsDataData->AnimNotifyWeaponAttack_HitActors.AddUnique(MeshComp->GetOwner());
	
	WeaponMeshComp = GetWeaponMeshComp(MeshComp);
	
	if (OwnerCharacter) //只有在Owner是Character的时候才执行检测(动画预览窗口中不会执行检测)
	{
		if (MeshComp->IsNetMode(NM_DedicatedServer))
		{
			//服务器需要恢复Pose来更新判定框的位置
			if (UWKSkeletalMeshComponent* DSSkeletalMeshComp = Cast<UWKSkeletalMeshComponent>(MeshComp))
			{
				DSSkeletalMeshComp->DedicatedServerRestoreCurrentPose(true);
			}
		}
		
		TArray<FHitResult> PendingHits;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(NotifyState_WeaponAttack), false, OwnerActor);

		TArray<AActor*> Children;
		OwnerActor->GetAttachedActors(Children);
		QueryParams.AddIgnoredActors(Children);
		for (TWeakObjectPtr<AActor> Item:HitActorsDataData->AnimNotifyWeaponAttack_HitActors)
		{
			if (Item.IsValid())
			{
				QueryParams.AddIgnoredActor(Item.Get());
			}
		}
		

		//忽略友方召唤物（包括自己）
		{
			// FDSTeamInfo TeamInfo = UDSTeamStatics::GetTeamInfo(OwnerActor);
			// for (const APlayerState* PlayerState : TeamInfo.PlayerTeamMembers)
			// {
			// 	if (IsValid(PlayerState))
			// 	{
			// 		ADSCharacter* Character = Cast<ADSCharacter>(PlayerState->GetPawn());
			// 		if (IsValid(Character) && IsValid(Character->GetSkillComponent()))
			// 		{
			// 			TArray<AActor*> SummonActors;
			// 			Character->GetSkillComponent()->GetAllSummonActors(SummonActors);
			// 			QueryParams.AddIgnoredActors(SummonActors);
			// 		}
			// 	}
			// }
			QueryParams.AddIgnoredActor(OwnerCharacter);
		}

		QueryParams.bReturnPhysicalMaterial = true;
		
		UWorld* MyWorld = MeshComp->GetWorld();

		if (bUseCustomDefinedCollision) //使用自定义形状进行碰撞检测
		{
			const int32 NumCustomCollisionShapes = CustomCollisionDefs.Num();
			for (int i = 0; i < NumCustomCollisionShapes; i++)
			{
				const FWeaponCustomCollisionDef& Def = CustomCollisionDefs[i];
				FTransform LocalTrans = WeaponMeshComp ? WeaponMeshComp->GetSocketTransform(Def.SocketName, RTS_Component) : MeshComp->GetSocketTransform(Def.SocketName, RTS_Component);
				LocalTrans = FTransform(Def.CollisionRotationOffset, Def.CollisionLocationOffset, FVector::OneVector) * LocalTrans;

				const FTransform Local2World = WeaponMeshComp ? WeaponMeshComp->GetComponentTransform() : MeshComp->GetComponentTransform();

				// 换算到世界坐标系
				FTransform Trans = LocalTrans * Local2World;

				// todo: HackFix，暴力保护性规避数组越界崩溃，先规避掉问题发包，后面细看
				if (LastCustomShapeTransform.IsValidIndex(i) && CustomCollisionShapes.IsValidIndex(i))
				{
					TArray<FHitResult> Hits;
					for (auto TraceChannel : TraceChannels)
					{
						if(Def.bUseMoreAttackRange)
						{
							FRotator WorldRotator = OwnerActor->GetActorRotation()+AttackRotationOffset + FRotator(0,-90,0);
							FVector HitDirection = UKismetMathLibrary::Conv_RotatorToVector(WorldRotator);
							
							if (MyWorld->SweepMultiByChannel(Hits, LastCustomShapeTransform[i].GetLocation(), Trans.GetLocation() + HitDirection * Def.MoreAttackRange, Trans.GetRotation(), TraceChannel, CustomCollisionShapes[i], QueryParams))
							{
								PendingHits.Append(Hits);
							}
						}
						else
						{
							if (MyWorld->SweepMultiByChannel(Hits, LastCustomShapeTransform[i].GetLocation(), Trans.GetLocation(), Trans.GetRotation(), TraceChannel, CustomCollisionShapes[i], QueryParams))
							{
								PendingHits.Append(Hits);
							}
						}
					}
					LastCustomShapeTransform[i] = Trans;
				}
				else
				{
					// DSLog(DSAnimNotifyState, Error,
					// 	TEXT("UDSAnimNotifyState_WeaponAttack: Invalid index: %d , LastCustomShapeTransform Num: %d, CustomCollisionShapes Num: %d"),
					// 	i,
					// 	LastCustomShapeTransform.Num(),
					// 	CustomCollisionShapes.Num()
					// );
				}
			}
		}
		else // 使用Mesh带的PhysicsAsset中的碰撞进行碰撞检测
		{
			if (TraceShapeData.Num() == 0)
			{
				// DSLog(DSAnimNotifyState, Error, TEXT("NotifyTick(%s): TraceShapeData is empty!"), *DSStringConversionHelper::NetRoleToString(MeshComp->GetOwner()->GetLocalRole()));
			}
			else
			{
				for (FSkeletalMeshBoneCollisionDataCache& ShapeData : TraceShapeData)
				{
					if (ShapeData.BoneIndex == INDEX_NONE)
					{
						// DSLog(DSAnimNotifyState, Error, TEXT("ShapeData has invalid BoneIndex!"));
					}

					if (ShapeData.CollisionShapes.Num() <= 0)
					{
						// DSLog(DSAnimNotifyState, Error, TEXT("ShapeData.CollisionShapes is empty!"));
					}
				}
			}

			for (FSkeletalMeshBoneCollisionDataCache& ShapeData : TraceShapeData)
			{
				if (ShapeData.BoneIndex != INDEX_NONE && ShapeData.CollisionShapes.Num() > 0)
				{
					const FTransform CurrentTrans = WeaponMeshComp->GetBoneTransform(ShapeData.BoneIndex);
					for (const FCollisionShapeCache& C : ShapeData.CollisionShapes)
					{
						const FTransform CollisionTrans = C.RelativeTransform * CurrentTrans;
						const FVector Start = (C.RelativeTransform * ShapeData.BoneTransform).GetTranslation();
						const FVector End = CollisionTrans.GetTranslation();
						const FQuat Rot = CollisionTrans.GetRotation();
						TArray<FHitResult> Hits;
						for (auto TraceChannel : TraceChannels)
						{
							if (MyWorld->SweepMultiByChannel(Hits, Start, End, Rot, TraceChannel, C.CollisionShape, QueryParams))
							{
								PendingHits.Append(Hits);
							}
						}
					}

					ShapeData.BoneTransform = CurrentTrans; // 更新缓存中的Transform
				}
			}
		}

		HandleHitActors(OwnerActor, PendingHits, EventReference);

	}
	else if (bUseCustomDefinedCollision) // 即使不执行检测，也需要更新自定义形状的Transform，这样在编辑器中才能绘制正确的 DEBUG 图形
	{
		const int32 NumCustomCollisionShapes = CustomCollisionDefs.Num();
		for (int i = 0; i < NumCustomCollisionShapes; i++)
		{
			const FWeaponCustomCollisionDef& Def = CustomCollisionDefs[i];
			FTransform LocalTrans = WeaponMeshComp ? WeaponMeshComp->GetSocketTransform(Def.SocketName, RTS_Component) : MeshComp->GetSocketTransform(Def.SocketName, RTS_Component);
			LocalTrans = FTransform(Def.CollisionRotationOffset, Def.CollisionLocationOffset, FVector::OneVector) * LocalTrans;

			const FTransform Local2World = WeaponMeshComp ? WeaponMeshComp->GetComponentTransform() : MeshComp->GetComponentTransform();

			// 换算到世界坐标系
			LastCustomShapeTransform[i] = LocalTrans * Local2World;
		}
	}


	// 调试武器碰撞
	// const float DebugDrawDuration = FWKWeaponConsoleVariables::CVarDebugMeleeCollision.GetValueOnAnyThread();
	const float DebugDrawDuration = 0;
	if (DebugDrawDuration > 0)
	{
		UWorld* MyWorld = MeshComp->GetWorld();
		if (bUseCustomDefinedCollision) // 绘制自定义形状
		{
			for (int i = 0; i < CustomCollisionDefs.Num(); i++)
			{
				// const FWeaponCustomCollisionDef& Def = CustomCollisionDefs[i];
				const FCollisionShape& Shape = CustomCollisionShapes[i];

				const FVector ShapeLocation = LastCustomShapeTransform[i].GetLocation();
				const FQuat ShapeRotation = LastCustomShapeTransform[i].GetRotation();

				if (Shape.IsBox())
				{
					DrawDebugBox(MyWorld, ShapeLocation, Shape.GetBox(), ShapeRotation, FColor::Red, false, DebugDrawDuration, 0, 1.f);
				}
				else if (Shape.IsSphere())
				{
					DrawDebugSphere(MyWorld, ShapeLocation, Shape.GetSphereRadius(), 24, FColor::Red, false, DebugDrawDuration, 0, 1.f);
				}
				else if (Shape.IsCapsule())
				{
					DrawDebugCapsule(MyWorld, ShapeLocation, Shape.GetCapsuleHalfHeight(), Shape.GetCapsuleRadius(), ShapeRotation, FColor::Red, false, DebugDrawDuration, 0, 1.f);
				}
			}
		}
		else
		{
			for (const FSkeletalMeshBoneCollisionDataCache& ShapeData : TraceShapeData)
			{
				FSkeletalMeshBoneCollisionDataCache Temp = ShapeData;
				Temp.BoneTransform = WeaponMeshComp->GetBoneTransform(ShapeData.BoneIndex);
				WKCollision::DrawDebugBoneCollisionData(MyWorld, Temp, FColor::Red, DebugDrawDuration);
			}
		}
	}
}

void UWKAnimNotifyState_MeleeAttackDetection::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr
		|| MeshComp->GetWorld() == nullptr
		|| MeshComp->GetOwner() == nullptr
		|| (UWKGameplayStatics::WorldIsGame(MeshComp) && MeshComp->GetOwner()->GetLocalRole() <= ROLE_SimulatedProxy))
	{
		return;
	}
	
	if (UWKSkeletalMeshComponent* WKSkeletalMeshComp = Cast<UWKSkeletalMeshComponent>(MeshComp))
	{
		WKSkeletalMeshComp->bEnableUpdateRateOptimizations = true;
		WKSkeletalMeshComp->SetServerUpdateBoneTransforms(false);
	}
	
	// 编辑器下保留引用
#if WITH_EDITOR
	if (UWKGameplayStatics::WorldIsGame(MeshComp))
	{
		OwnerMeshComp = nullptr;
		WeaponMeshComp = nullptr;
	}
	if (HitDirectionArrow)
	{
		HitDirectionArrow->DestroyComponent();
		HitDirectionArrow = nullptr;
	}

	/*
	TInlineComponentArray<UShapeComponent*, 8> ShapeComponentArray;
	GetCandidateComponents(*MeshComp, ShapeComponentArray);
	for (UShapeComponent* S : ShapeComponentArray)
	{
		S->SetVisibility(false);
	}
	*/

#else
	WeaponMeshComp = nullptr;
	SetWeaponMeshComp(MeshComp, WeaponMeshComp);
#endif
	

	TraceShapeData.Empty();

	//DebugCollisionShapes.Empty();
}

void UWKAnimNotifyState_MeleeAttackDetection::HandleHitActors(AActor* OwnerActor, TArray<FHitResult> PendingHits, const FAnimNotifyEventReference& EventReference)
{
	AWKCharacterBase* OwnerCharacter = Cast<AWKCharacterBase>(OwnerActor);
	if (!OwnerCharacter)
	{
		return;
	}
	UAbilitySystemComponent* OwnerASC = OwnerCharacter->GetAbilitySystemComponent();
	for (const FHitResult& Hit : PendingHits)
	{
		AActor* HitActor = Hit.GetActor();
		
		//if(!IsValid(Cast<ADSCharacter>(HitActor))) continue;
			
		if (HitActor && OwnerActor && HitActor != OwnerActor)
		{
			check(HitActor != OwnerActor);

			FANSHitActorsData* HitActorsData = DataContainer.GetOrAdd(Cast<ACharacter>(OwnerActor)->GetMesh());
			if(!HitActorsData->AnimNotifyWeaponAttack_HitActors.Contains(HitActor)/* can only hit this actor once */)
			{
				bool bReceiveHit = true;

				if (bCanDamageToFriendlyActor /* && UDSTeamStatics::IsTeammate(OwnerActor,HitActor) */)
				{
					bReceiveHit = true;
				}

				
				AWKCharacterBase* HitCharacter = Cast<AWKCharacterBase>(Hit.GetActor());
				if (IsValid(HitCharacter))
				{
					//无敌直接忽略
					if(HitCharacter->HasMatchingGameplayTag(WKGameplayTags::Gameplay_State_Invincible))
					{
						HitActorsData->AnimNotifyWeaponAttack_HitActors.AddUnique(HitActor);
						bReceiveHit = false;
					}
				}

				// 能接收伤害
				if (bReceiveHit)
				{
					if (EventTag.IsValid())
					{
						FGameplayEventData PayLoad;
						PayLoad.EventTag = EventTag;
						PayLoad.Instigator = OwnerActor;
						if(IsValid(EventReference.GetSourceObject()) && EventReference.GetNotify()!=nullptr)
						{
							PayLoad.EventMagnitude = EventReference.GetNotify()->GetTriggerTime();
						}
						FGameplayEffectContextHandle GEContextHandle = OwnerASC->MakeEffectContext();
						GEContextHandle.AddInstigator(OwnerActor->GetInstigatorController(), OwnerActor);
						GEContextHandle.AddHitResult(Hit);
						PayLoad.ContextHandle = GEContextHandle;
						PayLoad.Target = HitActor;
						PayLoad.TargetData.Add(new FGameplayAbilityTargetData_SingleTargetHit(Hit));
						// PayLoad.TargetData.Add(new FGameplayAbilityTargetData_Integer(((int32)WieldType)));
						PayLoad.InstigatorTags.AddTag(PayLoadTag);
						UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, PayLoad);

						if(ActionDerivationTag.IsValid())
						{
							PayLoad.EventTag = ActionDerivationTag;
							UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, ActionDerivationTag, PayLoad);
						}
					}

					//主控端需要发RPC给服务器通知hit
					if (OwnerActor->GetLocalRole() == ROLE_AutonomousProxy)
					{
						//TODO 近战判定
						// GetCreaturePlayerController(OwnerCreature)->ServerHandleMeleeHit(EventTag,Hit,HitFXCueTag,HitFXCueTag_KeyTag,bHitFXAttachToTarget,AttackRotationOffset,HitCurve,WieldType, AdditiveHitAnimImpact, PayLoadTag);
					}
					else
					{
						//AI的直接在服务器上运行
						if(IsValid(HitCharacter))
						{
							FRotator WorldRotator = OwnerActor->GetActorRotation()+AttackRotationOffset + FRotator(0,-90,0);
							FVector HitDirection = UKismetMathLibrary::Conv_RotatorToVector(WorldRotator);
		
							//击中特效
							// if (HitFXCueTag.IsValid() && HitFXCueTag != FGameplayTag::EmptyTag)
							// {
							// 	FGameplayCueParameters GCParams;
							// 	OwnerASC->InitDefaultGameplayCueParameters(GCParams);
							// 	GCParams.Instigator = OwnerActor;
							// 	GCParams.SourceObject = HitCharacter;
							// 	GCParams.Location = Hit.Location;
							//
							// 	FTransform BoneTransform;
							// 	if (UDSAnimationFunctionLibrary::GetBoneTransformByName(BoneTransform,HitCharacter->GetMesh(),Hit.BoneName,EBoneSpaces::WorldSpace))
							// 	{
							// 		GCParams.Location = BoneTransform.GetLocation();
							// 	}
							// 	if(bHitFXAttachToTarget)
							// 	{
							// 		GCParams.TargetAttachComponent = HitCharacter->GetMesh();
							// 		GCParams.RawMagnitude = HitCharacter->GetMesh()->GetBoneIndex(Hit.BoneName);
							// 	}
							// 	GCParams.AggregatedSourceTags.AddTag(HitFXCueTag_KeyTag);
							// 	GCParams.Normal = HitDirection;
							// 	GCParams.PhysicalMaterial = Hit.PhysMaterial;
							// 	OwnerASC->ExecuteGameplayCue(HitFXCueTag, GCParams);
							// }

							//受击动画
							// if(HitCurve)
							// {
							// 	//程序化受击
							// 	HitCharacter->PlayHitReactCurveSequence(HitCurve,-HitDirection);
							// }
							// else
							// {
							// 	// //叠加动画
							// 	// if (HitCharacter->CanPlayAdditiveHitReactAnimation(Hit.BoneName))
							// 	// {
							// 	// 	HitCharacter->PlayAdditiveHitReactAnimation(Hit.BoneName,HitDirection,HitCharacter->GetCurrentApplyDamageHitType(), AdditiveHitAnimImpact);
							// 	// }
							// }
						}
					}

					if (!bMultiHit)
					{
						HitActorsData->AnimNotifyWeaponAttack_HitActors.AddUnique(HitActor);
					}
				}
			}
		}
	}
}

TArray<FTransform>& UWKAnimNotifyState_MeleeAttackDetection::GetLastCustomShapeTransform(USkeletalMeshComponent* MeshComp)
{
	FWeaponAttackInfoDef& InfoDef = AttackInfoMap.FindOrAdd(MeshComp);
	return InfoDef.LastCustomShapeTransform;
}

const TArray<TObjectPtr<AActor>>& UWKAnimNotifyState_MeleeAttackDetection::GetHitActors(USkeletalMeshComponent* MeshComp)
{
	FWeaponAttackInfoDef& InfoDef = AttackInfoMap.FindOrAdd(MeshComp);
	return InfoDef.HitActors;
}

#if WITH_EDITORONLY_DATA
void UWKAnimNotifyState_MeleeAttackDetection::CreateDebugShapesForCustomCollision(USkeletalMeshComponent* OwnerComponent, const TArray<FWeaponCustomCollisionDef>& InCollisionDefs)
{
	if (!OwnerComponent)
	{
		return;
	}

	TInlineComponentArray<UShapeComponent*, 8> ShapeComponentArray;
	GetCandidateComponents(*OwnerComponent, ShapeComponentArray);
	for (UShapeComponent* S : ShapeComponentArray)
	{
		S->SetVisibility(false);
	}

	for (const FWeaponCustomCollisionDef& Def : InCollisionDefs)
	{
		UShapeComponent* NewShape;
		switch (Def.CollisionType)
		{
		case EWeaponCollisionType::Capsule:
			{
				UCapsuleComponent* Capsule = FindAndRemoveFrom<UCapsuleComponent>(ShapeComponentArray);
				if (Capsule == nullptr)
				{
					Capsule = NewObject<UCapsuleComponent>(OwnerComponent);
				}
				Capsule->SetCapsuleRadius(FMath::Max(Def.CollisionExtent.X, Def.CollisionExtent.Y));
				Capsule->SetCapsuleHalfHeight(Def.CollisionExtent.Z / 2);
				NewShape = Capsule;
				break;
			}
		case EWeaponCollisionType::Sphere:
			{
				USphereComponent* Sphere = FindAndRemoveFrom<USphereComponent>(ShapeComponentArray);
				if (Sphere == nullptr)
				{
					Sphere = NewObject<USphereComponent>(OwnerComponent);
				}
				Sphere->SetSphereRadius(FMath::Max3(Def.CollisionExtent.X, Def.CollisionExtent.Y, Def.CollisionExtent.Z));
				NewShape = Sphere;
				break;
			}
		default:
			{
				UBoxComponent* Box = FindAndRemoveFrom<UBoxComponent>(ShapeComponentArray);
				if (Box == nullptr)
				{
					Box = NewObject<UBoxComponent>(OwnerComponent);
				}
				Box->SetBoxExtent(Def.CollisionExtent / 2);
				NewShape = Box;
				break;
			}
		}

		check(NewShape);
		if (!NewShape->IsRegistered())
		{
			NewShape->RegisterComponentWithWorld(OwnerComponent->GetWorld());
		}
		NewShape->SetVisibility(true);
		NewShape->SetWorldScale3D(FVector::One());
		NewShape->SetUsingAbsoluteScale(true);
		NewShape->AttachToComponent(WeaponMeshComp ? WeaponMeshComp.Get() : OwnerComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Def.SocketName);
		NewShape->SetRelativeLocationAndRotation(Def.CollisionLocationOffset, Def.CollisionRotationOffset, false);
	}


	if (!IsValid(HitDirectionArrow))
	{
		HitDirectionArrow = NewObject<UArrowComponent>(OwnerComponent);
	}
	
	HitDirectionArrow->SetWorldScale3D(FVector::One());
	HitDirectionArrow->SetRelativeLocationAndRotation(FVector::ZeroVector, AttackRotationOffset, false);
		
	if (!HitDirectionArrow->IsRegistered())
	{
		HitDirectionArrow->RegisterComponentWithWorld(OwnerComponent->GetWorld());
	}
	HitDirectionArrow->SetVisibility(true);
}

void UWKAnimNotifyState_MeleeAttackDetection::CreateDebugShapesForCollisionData(USkeletalMeshComponent* OwnerComponent, const TArray<FSkeletalMeshBoneCollisionDataCache> InCollisionData)
{
	if (!OwnerComponent)
	{
		return;
	}

	TInlineComponentArray<UShapeComponent*, 8> ShapeComponentArray;
	GetCandidateComponents(*OwnerComponent, ShapeComponentArray);
	for (UShapeComponent* S : ShapeComponentArray)
	{
		S->SetVisibility(false);
	}

	for (const FSkeletalMeshBoneCollisionDataCache& ShapeData : TraceShapeData)
	{
		for (const FCollisionShapeCache& C : ShapeData.CollisionShapes)
		{
			UShapeComponent* NewShape = nullptr;
			if (C.CollisionShape.IsBox())
			{
				UBoxComponent* Box = FindAndRemoveFrom<UBoxComponent>(ShapeComponentArray);
				if (Box == nullptr)
				{
					Box = NewObject<UBoxComponent>(OwnerComponent);
				}
				Box->SetBoxExtent(C.CollisionShape.GetBox());
				NewShape = Box;
			}
			else if (C.CollisionShape.IsSphere())
			{
				USphereComponent* Sphere = FindAndRemoveFrom<USphereComponent>(ShapeComponentArray);
				if (Sphere == nullptr)
				{
					Sphere = NewObject<USphereComponent>(OwnerComponent);
				}
				Sphere->SetSphereRadius(C.CollisionShape.GetSphereRadius());
				NewShape = Sphere;
			}
			else if (C.CollisionShape.IsCapsule())
			{
				UCapsuleComponent* Capsule = FindAndRemoveFrom<UCapsuleComponent>(ShapeComponentArray);
				if (Capsule == nullptr)
				{
					Capsule = NewObject<UCapsuleComponent>(OwnerComponent);
				}
				Capsule->SetCapsuleHalfHeight(C.CollisionShape.GetCapsuleHalfHeight());
				Capsule->SetCapsuleRadius(C.CollisionShape.GetCapsuleRadius());
				NewShape = Capsule;
			}

			check(NewShape);
			if (!NewShape->IsRegistered())
			{
				NewShape->RegisterComponentWithWorld(OwnerComponent->GetWorld());
			}
			NewShape->SetVisibility(true);
			NewShape->AttachToComponent(WeaponMeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponMeshComp->GetBoneName(ShapeData.BoneIndex));
			NewShape->SetRelativeTransform(C.RelativeTransform);
		}
	}
}
#endif

USkeletalMeshComponent* UWKAnimNotifyState_MeleeAttackDetection::GetWeaponMeshComp(USkeletalMeshComponent* MeshComp)
{
	FWeaponAttackInfoDef& InfoDef = AttackInfoMap.FindOrAdd(MeshComp);
	return InfoDef.WeaponMeshComp;
}

void UWKAnimNotifyState_MeleeAttackDetection::SetWeaponMeshComp(USkeletalMeshComponent* MeshComp, USkeletalMeshComponent* InWeaponMeshComp)
{
	FWeaponAttackInfoDef& InfoDef = AttackInfoMap.FindOrAdd(MeshComp);
	InfoDef.WeaponMeshComp = InWeaponMeshComp;
}
