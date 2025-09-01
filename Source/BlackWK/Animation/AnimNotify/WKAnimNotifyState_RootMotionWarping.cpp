// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotifyState_RootMotionWarping.h"

#include "BlackWK/Character/WKCharacterBase.h"
#include "BlackWK/Utils/WKMath.h"

static TAutoConsoleVariable<int32> CVarDrawRootMotionPlayerCameraInfo(
	TEXT("wk.DrawRootMotionPlayerCameraInfo"),
	0,
	TEXT("显示基于角色视口选择目标的调试信息。0：关闭绘制，1: 开启绘制 \n"),
	ECVF_Default
);

AActor* UWKTargetCatchRule_AIBlackboardTargetActor::TargetSearch_Implementation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability)
{
	// return GetHightestThreatTarget(SelfCharacter);
	return nullptr;
}

AActor* UWKTargetCatchRule_PlayerCamera::TargetSearch_Implementation(AWKCharacterBase* SelfCharacter, UGameplayAbility* Ability)
{
	if (!IsValid(SelfCharacter))
	{
		return nullptr;
	}

	// if (SelfCharacter->IsRLCharacter())
	// {
	// 	if (const UDSAIManager* AIManager = UDSAIManager::GetDSAIManager(SelfCharacter))
	// 	{
	// 		if (const AActor* ControlTowardTarget = AIManager->GetControlTowardTarget(SelfCharacter))
	// 		{
	// 			return const_cast<AActor*>(ControlTowardTarget);
	// 		}
	// 	}
	// }

	// if (SelfCharacter->IsPlayerAICharacter())
	// {
	// 	// 如果该AI可以被认作是“玩家”，则允许直接返回黑板中的TargetActor为Warping目标
	// 	return GetHightestThreatTarget(SelfCharacter);
	// }


	// if (!SelfCharacter->IsPlayerControlled())
	// {
	// 	return nullptr;
	// }
	//
	// AWKCharacterBase* PlayerSelfCharacter = SelfCharacter;
	//
	// if (!IsValid(PlayerSelfCharacter->GetPlayerController())||!IsValid(PlayerSelfCharacter->GetPlayerController()->PlayerCameraManager))
	// {
	// 	return nullptr;
	// }
	//
	// //“致盲”状态下，无法修正
	// if (SelfCharacter->HasMatchingGameplayTag(UDSGameplayTags::Get().Blindness))
	// {
	// 	return nullptr;
	// }
	//
	// FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(UWKTargetCatchRule_PlayerCamera), false, SelfCharacter);
	// TArray<AActor*> AttachedActors;
	// SelfCharacter->GetAttachedActors(AttachedActors);
	// QueryParams.AddIgnoredActors(AttachedActors);
	//
	// //锁敌距离增加
	// float FinalSearchDistance = SearchDistance;
	// if(PlayerSelfCharacter->GetAttribute()->GetTalent_WrapingSpecificityDistance() > 0 && bApplySpecificity)
	// {
	// 	FinalSearchDistance += PlayerSelfCharacter->GetAttribute()->GetTalent_WrapingSpecificityDistance();
	// }
	// FinalSearchDistance += PlayerSelfCharacter->GetAttribute()->GetWrapingDistanceAdditive();
	// //相机拉远后，需要维持索敌距离
	// if (PlayerSelfCharacter->GetDSCameraComponent())
	// {
	// 	FinalSearchDistance += PlayerSelfCharacter->GetDSCameraComponent()->CurrentZoomOut;
	// }
	//
	// const  FVector StartLocation = PlayerSelfCharacter->GetDSCameraComponent()->GetComponentLocation();
	//
	// TArray<AActor*> DamageableActors;
	// UDSWorldDamageableActorManager::Get(SelfCharacter)->GetAllDamageableActor(DamageableActors);
	//
	// if (CVarDrawRootMotionPlayerCameraInfo.GetValueOnGameThread())
	// {
	// 	DrawDebugSphere(SelfCharacter->GetWorld(),StartLocation,FinalSearchDistance,16, FColor::White, false, 3.f, 0, 1.f);
	// }
	//
	// //视野范围
	// const float FOVAngle = PlayerSelfCharacter->GetPlayerController()->PlayerCameraManager->GetFOVAngle();
	// TArray<AActor*> Candidates;
	// int32 SearchPriority = 0;
	// for (int i = 0; i < DamageableActors.Num(); i++)
	// {
	// 	AActor* CandidateActor = DamageableActors[i];
	// 	if (CandidateActor == SelfCharacter)
	// 	{
	// 		continue;
	// 	}
	// 	if(!USDMathBlueprintLibrary::K2_IsPointInsideSphere(CandidateActor->GetActorLocation(), StartLocation, FinalSearchDistance))
	// 	{
	// 		continue;
	// 	}
	// 	
	// 	if (IsValid(CandidateActor))
	// 	{
	// 		//优先级对比
	// 		UDSDamageableComponent* DamageableComponent = CandidateActor->GetComponentByClass<UDSDamageableComponent>();
	// 		if (!DamageableComponent)
	// 		{
	// 			continue;				
	// 		}
	// 		int32 Priority = DamageableComponent->GetSearchPriority();
	// 		if (Priority < SearchPriority )
	// 		{
	// 			continue;
	// 		}
	//
	// 		FDSCreature CandidateCreature(CandidateActor);
	// 		if (IsValidCreature(CandidateCreature))
	// 		{
	// 			if (IsDead(CandidateCreature))
	// 			{
	// 				continue;
	// 			}
	// 			
	// 			if (HasMatchingGameplayTag(CandidateCreature,UDSGameplayTags::Get().State_Stealth))
	// 			{
	// 				continue;
	// 			}
	//
	// 			if (HasMatchingGameplayTag(CandidateCreature,UDSGameplayTags::Get().InBush))
	// 			{
	// 				continue;
	// 			}
	// 			
	// 			if (!UDSTeamStatics::IsTeammate(SelfCharacter, CandidateActor))
	// 			{
	// 				FVector TargetCharacterVector = CandidateActor->GetActorLocation();
	// 				FVector EnemyVector = TargetCharacterVector - StartLocation;
	// 				FVector ControlVector = SelfCharacter->GetControlDirection();
	// 				//忽略俯仰角
	// 				const float Angle = FDSMath::AngleBetweenVectors(EnemyVector.GetSafeNormal2D(), ControlVector.GetSafeNormal2D());
	//
	// 				if (bCheckNoObstacle)
	// 				{
	// 					FHitResult HitResult;
	// 					
	// 					SelfCharacter->GetWorld()->LineTraceSingleByChannel(HitResult,SelfCharacter->GetActorLocation(),CandidateActor->GetActorLocation(),DSCollisionChannel::Environment,QueryParams);
	//
	// 					if (CVarDrawRootMotionPlayerCameraInfo.GetValueOnGameThread())
	// 					{
	// 						DrawDebugLine(SelfCharacter->GetWorld(),SelfCharacter->GetActorLocation(),HitResult.TraceEnd,FColor::Blue,false,3.f,SDPG_World,5);
	// 					}
	// 					
	// 					//检测到目标点之间有障碍
	// 					if (HitResult.bBlockingHit == true)
	// 					{
	// 						continue;
	// 					}
	// 				}
	// 				if (FMath::Abs(Angle) <= (FOVAngle / 2))
	// 				{
	// 					if (DamageableComponent->CanSearchBy(SelfCharacter))
	// 					{
	// 						if (ACharacter* CandidateCharacter = Cast<ACharacter>(CandidateActor))
	// 						{
	// 							if(IsValid(CandidateCharacter->GetMesh()) && IsValid(CandidateCharacter->GetMesh()->GetSkeletalMeshAsset()) && CandidateCharacter->GetMesh()->IsVisible())
	// 							{
	// 								//被渲染的才选取，避免忽略了俯仰角造成选到屏幕外面的
	// 								if (!CandidateCharacter->GetMesh()->WasRecentlyRendered(0.1f))
	// 								{
	// 									continue;
	// 								}	
	// 							}
	// 						}
	// 							
	// 						if (Priority > SearchPriority)
	// 						{
	// 							//更新优先级,清空低优先级内容
	// 							SearchPriority = Priority;
	// 							Candidates.Empty();
	// 						}
	// 						Candidates.Add(CandidateActor);
	// 					}
	// 				}
	// 			}
	// 		}
	// 		else
	// 		{
	// 			if (DamageableComponent->CanSearchBy(SelfCharacter))
	// 			{
	// 				if (Priority > SearchPriority)
	// 				{
	// 					//更新优先级,清空低优先级内容
	// 					SearchPriority = Priority;
	// 					Candidates.Empty();
	// 				}
	// 				Candidates.Add(CandidateActor);
	// 			}
	// 		}
	// 	}
	// }
	//
	// if (CVarDrawRootMotionPlayerCameraInfo.GetValueOnGameThread())
	// {
	// 	DrawDebugDirectionalArrow(SelfCharacter->GetWorld(), StartLocation, StartLocation +  SelfCharacter->GetControlDirection().GetSafeNormal2D().RotateAngleAxis(-FMath::Abs(FOVAngle / 2), SelfCharacter->GetActorUpVector())*SearchDistance, 10, FColor::Orange, false, 3.0f);
	// 	DrawDebugDirectionalArrow(SelfCharacter->GetWorld(), StartLocation, StartLocation +  SelfCharacter->GetControlDirection().GetSafeNormal2D().RotateAngleAxis(FMath::Abs(FOVAngle / 2), SelfCharacter->GetActorUpVector())*SearchDistance, 10, FColor::Orange, false, 3.0f);
	// }
	//
	// if (Candidates.IsEmpty())
	// {
	// 	return nullptr;
	// }
	// if (Candidates.Num() == 1)
	// {
	// 	return Candidates[0];
	// }
	//
	// TMap<AActor*,float> CandidateSD;
	//
	// //找出最小SD
	// float MinSD = -1;
	// for (AActor* Candidate : Candidates)
	// {
	// 	FVector EnemyVector = Candidate->GetActorLocation() - SelfCharacter->GetAimViewLocation();
	// 	FVector AimVector = SelfCharacter->GetBaseAimRotation().Vector();
	// 	
	// 	float Angle = FMath::Abs(FWKMath::AngleBetweenVectors(EnemyVector, AimVector));
	// 	float ScreenDistance = EnemyVector.Length() * FMath::Sin(FMath::DegreesToRadians(Angle));
	//
	// 	if (MinSD == -1 || ScreenDistance < MinSD)
	// 	{
	// 		MinSD = ScreenDistance;
	// 	}
	//
	// 	CandidateSD.Add(Candidate,ScreenDistance);
	// }
	//
	// //比较所有目标与SD最小值的差是否在屏幕距离容差SDTolerance范围内, 若是, 则这些目标的SD = Avg(MiniSD(即SD0), SD1, SD2...)
	// float AvgSD = 0.f;
	// int32 Num = 0;
	// for (auto Item:CandidateSD)
	// {
	// 	if (FMath::Abs(Item.Value - MinSD)<= SDTolerance)
	// 	{
	// 		AvgSD += Item.Value;
	// 		Num++;
	// 	}
	// }
	// AvgSD /= Num;
	//
	// AActor* SelectedTarget = nullptr;
	// float MaxResult = -1;
	// if (!Candidates.IsEmpty())
	// {
	// 	for (AActor* Candidate : Candidates)
	// 	{
	// 		float ScreenDistance = CandidateSD[Candidate];
	// 		if (FMath::Abs(ScreenDistance - MinSD)<= SDTolerance)
	// 		{
	// 			ScreenDistance = AvgSD;
	// 		}
	// 		float WorldDistance = FVector::Distance(StartLocation, Candidate->GetActorLocation());
	// 		//((1 / SD) * SDMultiply) + ((1 / D) * DMultiply)
	// 		float Result = ((1.f / FMath::Max(ScreenDistance, 0.01f)) * SDMultiply) + ((1.f / FMath::Max(WorldDistance, 0.01f)) * DMultiply);
	//
	// 		if (CVarDrawRootMotionPlayerCameraInfo.GetValueOnGameThread())
	// 		{
	// 			DrawDebugBox(SelfCharacter->GetWorld(), Candidate->GetActorLocation(), FVector(25.f), FColor::Red, false, 3.f, 0, 2.f);
	// 			// DrawDebugDirectionalArrow(
	// 			// 	SelfCharacter->GetWorld(),
	// 			// 	SelfCharacter->Controller->K2_GetActorLocation(),
	// 			// 	SelfCharacter->Controller->K2_GetActorLocation() + AimVector * 300.f,
	// 			// 	8.f, FColor::Yellow, false, 5.f, 0.f, 2.f
	// 			// 	);
	// 			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::White,
	// 			                                 FString::Printf(TEXT("((1 / %f) * %f) + ((1 / %f) * %f) = %f"), ScreenDistance, SDMultiply, WorldDistance, DMultiply, Result));
	// 		}
	//
	// 		if (MaxResult == -1 || Result > MaxResult)
	// 		{
	// 			SelectedTarget = Candidate;
	// 			MaxResult = Result;
	// 		}
	// 	}
	// }
	//
	// if (CVarDrawRootMotionPlayerCameraInfo.GetValueOnGameThread())
	// {
	// 	if (SelectedTarget)
	// 		DrawDebugBox(SelfCharacter->GetWorld(), SelectedTarget->GetActorLocation(), FVector(20.f), FColor::Green, false, 3.f, 0, 2.f);
	// }
	//
	// return SelectedTarget;
	return nullptr;
}


UWKAnimNotifyState_RootMotionWarping::UWKAnimNotifyState_RootMotionWarping()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor::Magenta;
#endif
}

void UWKAnimNotifyState_RootMotionWarping::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UWKAnimNotifyState_RootMotionWarping::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}