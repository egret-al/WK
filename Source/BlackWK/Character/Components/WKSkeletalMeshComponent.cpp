// Fill out your copyright notice in the Description page of Project Settings.


#include "WKSkeletalMeshComponent.h"

#include "BlackWK/Animation/WKAnimationFunctionLibrary.h"
#include "GameFramework/Character.h"

static TAutoConsoleVariable<float> CVarDrawServerRestoreCurrentPose(
	TEXT("ds.DrawServerRestoreCurrentPose"),
	0,
	TEXT("绘制服务器恢复的Pose 参数为持续的秒数\n"),
	ECVF_Default
);

UWKSkeletalMeshComponent::UWKSkeletalMeshComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, NetMode()
{
	bDisableRootMotion = true;
	bServerRestoringPose = false;

	//性能优化选项
	bEnableUpdateRateOptimizations = true;
	bDeferKinematicBoneUpdate = true;
	//bUseBoundsFromLeaderPoseComponent = true;
	//bComponentUseFixedSkelBounds = true;
	bSkipKinematicUpdateWhenInterpolating = true;
	bSkipBoundsUpdateWhenInterpolating = true;

	//会导致模型模糊
	//bPerBoneMotionBlur = false;
}

void UWKSkeletalMeshComponent::BeginPlay()
{
	NetMode = GetWorld()->GetNetMode();

	if (NetMode == ENetMode::NM_DedicatedServer)
	{
		if (GetAttachParent() && GetAttachParent()->IsA(USkeletalMeshComponent::StaticClass()))
		{
			//挂载在其他SkeletalMeshComponent上的子Mesh都不更新
			VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesWhenNotRendered;
		}
		else
		{
			SetServerUpdateBoneTransforms(bServerUpdateBoneTransforms);
		}

		//不运行后处理动画蓝图
		SetDisablePostProcessBlueprint(true);

		if (bServerNoSkeletonUpdateWhenIsNotCharacter)
		{
			if (!GetOwner()->IsA(ACharacter::StaticClass()))
			{
				bNoSkeletonUpdate = true;
			}
		}
	}
	else if (NetMode == ENetMode::NM_Client)
	{
		if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
		{
			//5.5之后如果开启了URO，并设置为OnlyTickMontagesAndRefreshBonesWhenPlayingMontages，没有渲染时候RefreshBoneTransforms会导致崩溃
			//VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesAndRefreshBonesWhenPlayingMontages;
			VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesWhenNotRendered;
		}
		else
		{
			VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		}
	}

	Super::BeginPlay();
}

void UWKSkeletalMeshComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UWKSkeletalMeshComponent::TickComponent);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (NetMode == ENetMode::NM_Client)
	{
		if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
		{
			if (!GetOwner()->WasRecentlyRendered())
			{
				bDisableClothSimulation = true;
			}
			else
			{
				bDisableClothSimulation = false;
			}
		}
	}
}

bool UWKSkeletalMeshComponent::ShouldUpdateTransform(bool bLODHasChanged) const
{
	if (NetMode == ENetMode::NM_DedicatedServer && !bServerUpdateBoneTransforms)
	{
		return false;
	}
	return Super::ShouldUpdateTransform(bLODHasChanged);
}

void UWKSkeletalMeshComponent::RefreshBoneTransforms(FActorComponentTickFunction* TickFunction)
{
#if WITH_EDITOR
	if (UWorld* World = GetWorld())
	{
		if (World->WorldType == EWorldType::Editor)
		{
			Super::RefreshBoneTransforms(TickFunction);
			return;
		}
	}
#endif
	if (LastRefreshBoneTransformsFrame == GFrameCounter)
	{
		return;
	}
	LastRefreshBoneTransformsFrame = GFrameCounter;

	if (NetMode == ENetMode::NM_DedicatedServer)
	{
		//服务器只更新角色的主mesh，其他附加在主mesh的上子Mesh均不更新
		if (GetAttachParent() && GetAttachParent()->IsA(USkeletalMeshComponent::StaticClass()))
		{
			return;
		}
	}
	Super::RefreshBoneTransforms(TickFunction);
}

void UWKSkeletalMeshComponent::FinalizeBoneTransform()
{
	if (NetMode == ENetMode::NM_DedicatedServer && bServerRestoringPose)
	{
		//恢复Pose时，跳过ConditionallyDispatchQueuedAnimEvents，避免恢复pose重复触发动画通知的问题
		return Super::Super::FinalizeBoneTransform();
	}
	Super::FinalizeBoneTransform();
}

bool UWKSkeletalMeshComponent::IsPlayingRootMotionFromEverything() const
{
	if (bDisableRootMotion)
	{
		return false;
	}
	return Super::IsPlayingRootMotionFromEverything();
}

void UWKSkeletalMeshComponent::DedicatedServerRestoreCurrentPose(bool ForceRestore)
{
	SCOPED_NAMED_EVENT(DedicatedServerRestoreCurrentPose, FColor::Red);
	if (NetMode == ENetMode::NM_DedicatedServer
		&& (bServerRestorePose || ForceRestore)
		&& !bServerUpdateBoneTransforms
		&& !IsPostEvaluatingAnimation()
		&& LastRefreshBoneTransformsFrame != GFrameCounter)
	{
		bServerRestoringPose = true;
		//bEnableUpdateRateOptimizations = false;
		RefreshBoneTransforms();
		//bEnableUpdateRateOptimizations = true;
		bServerRestoringPose = false;

		if (CVarDrawServerRestoreCurrentPose.GetValueOnGameThread() > 0)
		{
			UWKAnimationFunctionLibrary::DrawDebugSkeletalMeshPose(this, EBoneSpaces::WorldSpace, CVarDrawServerRestoreCurrentPose.GetValueOnGameThread(), FColor::Green);
		}
	}
}

bool UWKSkeletalMeshComponent::IsPlayingRootMotion_Direct() const
{
	return (Super::IsPlayingRootMotionFromEverything() || IsPlayingNetworkedRootMotionMontage());
}

void UWKSkeletalMeshComponent::SetDisableRootMotion(bool IsDisable)
{
	bDisableRootMotion = IsDisable;
}

void UWKSkeletalMeshComponent::SetServerUpdateBoneTransforms(bool NewServerUpdateBoneTransforms)
{
	if (NetMode == ENetMode::NM_DedicatedServer)
	{
		bServerUpdateBoneTransforms = NewServerUpdateBoneTransforms;
		if (bServerUpdateBoneTransforms)
		{
			//会对服务器造成巨量的性能消耗，未经程序评估严禁开启
			VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		}
		else
		{
			if (bServerRestorePose && bServerRestorePosePrecise)
			{
				//精确恢复Pose，需要TickPose，但是会通过EnableUpdateRateOptimizations = true来降低更新频率
				VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
			}
			else
			{
				//只跑蒙太奇通知逻辑
				VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesWhenNotRendered;
			}
		}
	}
}