// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "WKSkeletalMeshComponent.generated.h"

/**
 *  此类主要目的是进行SkeletalMesh的性能优化，特别是服务器动画性能
 *  
 *  对于服务器，bServerUpdateBoneTransforms默认为false(未经程序评估严禁设置为true)，会关闭Tick更新骨骼的Transform，能节约大量的性能
 *  在服务器需要恢复Pose时，调用 UBDSkeletalMeshComponent::DedicatedServerRestoreCurrentPose()会恢复到当前帧Pose的骨骼位置;
 *  
 *  bServerRestorePosePrecise 默认为false，只能恢复Montage里原始Sequence的Pose（不带瞄准偏移等程序化效果） 和 动画蓝图里Idle状态的Pose
 *	bServerRestorePosePrecise 设置为true时，可以恢复到和客户端一样的Pose，但代价是服务器需要更新动画数据，非必要不要开启
 *
 *  对于客户端，会根据是主控端还是模拟端、LOD级别、是否被渲染来降低动画更新频率
 */
UCLASS(Blueprintable, EditInlineNew, ShowCategories = (Mobility), Config = Engine, meta = (BlueprintSpawnableComponent))
class BLACKWK_API UWKSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UWKSkeletalMeshComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	//~ Begin SkeletalMeshComponent Interface.
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool ShouldUpdateTransform(bool bLODHasChanged) const override;
	virtual void RefreshBoneTransforms( FActorComponentTickFunction* TickFunction = NULL ) override;
	virtual void FinalizeBoneTransform() override;
	virtual bool IsPlayingRootMotionFromEverything() const override;
	//~ End SkeletalMeshComponent Interface.
	
	/**
	 * 服务器恢复当前Pose的骨骼位置
	 *  @ForceRestore 即便没有启用bServerRestorePose，也强制调用恢复Pose
	*/
	void DedicatedServerRestoreCurrentPose(bool ForceRestore = false);
	
	virtual bool IsPlayingRootMotion_Direct() const;

	UFUNCTION(BlueprintCallable)
	void SetDisableRootMotion(bool IsDisable);
	
	/** 
	 *  服务器是否tick更新骨骼位置
	 *  会对服务器造成巨量的性能消耗，未经程序评估严禁开启
	 */
	UPROPERTY(EditDefaultsOnly, Category = "WKSkeletalMeshComponent")
	bool bServerUpdateBoneTransforms = false;
	
	/** 
	 *  打开/关闭服务器上Tick更新骨骼位置
	 *  用于一段时间需要连续更新骨骼位置的情况（如果只是某一帧需要取骨骼位置的话，别调用这个，调用 DedicatedServerRestoreCurrentPose恢复一帧pose就行）
	 *  会对服务器造成大量的性能消耗，严禁长期开启，不需要的时候要及时关闭
	 */
	UFUNCTION(BlueprintCallable, Category = "WKSkeletalMeshComponent")
	void SetServerUpdateBoneTransforms(bool NewServerUpdateBoneTransforms);

	/**
	 *  是否启用服务器恢复Pose功能
	 */
	UPROPERTY(EditDefaultsOnly, Category = "WKSkeletalMeshComponent|Config", meta = (EditCondition = "!bServerUpdateBoneTransforms"))
	bool bServerRestorePose = false;
	
	/** 服务器恢复Pose时，是否需要恢复更加精准的pose
	 *  bServerRestorePosePrecise 默认为false，只能恢复Montage里原始Sequence的Pose（不带瞄准偏移等程序化效果） 和 动画蓝图里Idle状态的Pose
	 *	bServerRestorePosePrecise 设置为true时，可以恢复到和客户端一样的Pose，但代价是服务器需要更新动画数据，非必要不要开启
	 */
	UPROPERTY(EditDefaultsOnly, Category = "WKSkeletalMeshComponent|Config", meta = (EditCondition = "!bServerUpdateBoneTransforms && bServerRestorePose"))
	bool bServerRestorePosePrecise = false;
	
	/** 当Owner不是Character时，服务器是否停止骨骼更新*/
	UPROPERTY(EditDefaultsOnly, Category = "WKSkeletalMeshComponent|Config")
	bool bServerNoSkeletonUpdateWhenIsNotCharacter = true;

private:
	bool bDisableRootMotion;
	uint64 LastRefreshBoneTransformsFrame;
	ENetMode NetMode;
	bool bServerRestoringPose = false;
};
