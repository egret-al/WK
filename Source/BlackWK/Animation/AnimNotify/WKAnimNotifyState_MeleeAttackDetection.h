// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WKAnimNotifyState.h"
#include "BlackWK/Gameplay/WKCollisionTypes.h"
#include "WKAnimNotifyState_MeleeAttackDetection.generated.h"

class UArrowComponent;
using namespace WKCollision;

/** 自定义形状类型 */
UENUM(BlueprintType)
enum class EWeaponCollisionType : uint8
{
	Box,
	Sphere,
	Capsule,
};


USTRUCT(BlueprintType)
struct FANSHitActorsData
{
	GENERATED_BODY()
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AActor>> AnimNotifyWeaponAttack_HitActors;
};

USTRUCT(BlueprintType)
struct FWeaponCustomCollisionDef
{
	GENERATED_BODY()
public:

	FWeaponCustomCollisionDef()
		: CollisionType (EWeaponCollisionType::Box)
		, CollisionExtent(FVector(10.f, 10.f, 30.f))
		, CollisionLocationOffset(ForceInit)
		, CollisionRotationOffset(ForceInit)
		, SocketName(TEXT("WeaponCollision"))
		, bUseMoreAttackRange(false)
		, MoreAttackRange(ForceInit)
	{};

	/** 自定义形状类型 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponCollisionType CollisionType;

	/** 自定义形状的大小 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CollisionExtent;

	/** 自定义形状的位置偏移 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CollisionLocationOffset;

	/** 自定义形状的旋转偏移 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator CollisionRotationOffset;

	/** 自定义形状的依附的socket */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;

	/** 攻击方向更大的攻击范围判定 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseMoreAttackRange;

	/** 攻击范围判定打的距离 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseMoreAttackRange", EditConditionHides))
	float MoreAttackRange;
};

USTRUCT(BlueprintType)
struct FWeaponAttackInfoDef
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComp;

	UPROPERTY(Transient)
	TArray<FTransform> LastCustomShapeTransform;

	/** 击中过的Actor */
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> HitActors;
};

/**
 * 伤害检测的动画通知
 */
UCLASS()
class BLACKWK_API UWKAnimNotifyState_MeleeAttackDetection : public UWKAnimNotifyState
{
	GENERATED_BODY()

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual void HandleHitActors(AActor* OwnerActor, TArray<FHitResult> PendingHits, const FAnimNotifyEventReference& EventReference);

#if WITH_EDITORONLY_DATA
protected:

	/** 用于在动画编辑器中预览的武器模型 */
	UPROPERTY(EditAnywhere, Category = "Preview")
	TObjectPtr<USkeletalMesh> WeaponPreviewMesh;

	/** 预览武器模型的插槽名称 */
	UPROPERTY(EditAnywhere, Category = "Preview")
	FName PreviewMeshSocketName;

	/** 预览武器模型的位置偏移 */
	UPROPERTY(EditAnywhere, Category = "Preview")
	FVector PreviewMeshLocationOffset;

	/** 预览武器模型的旋转偏移 */
	UPROPERTY(EditAnywhere, Category = "Preview")
	FRotator PreviewMeshRotationOffset;

	UPROPERTY(Transient)
	TArray<TObjectPtr<class UShapeComponent>> DebugCollisionShapes;

	UPROPERTY()
	TObjectPtr<UArrowComponent> HitDirectionArrow;

private:
	TArray<FTransform>& GetLastCustomShapeTransform(USkeletalMeshComponent* MeshComp);
	const TArray<TObjectPtr<AActor>>& GetHitActors(USkeletalMeshComponent* MeshComp);
	void CreateDebugShapesForCustomCollision(class USkeletalMeshComponent* OwnerComponent, const TArray<FWeaponCustomCollisionDef>& InCollisionDefs);
	void CreateDebugShapesForCollisionData(class USkeletalMeshComponent* OwnerComponent, const TArray<FSkeletalMeshBoneCollisionDataCache> InCollisionData);

#endif
	
protected:
	/** 是否允许多次击中检测 */
	UPROPERTY(EditAnywhere, Category = "WeaponAttack")
	bool bMultiHit;
	
	/** 动作派生tag，近战命中后发送次tag的event */
	UPROPERTY(EditAnywhere, Category = "WeaponAttack")
	FGameplayTag ActionDerivationTag;
	
	/** 碰撞检测成功后发送的GameplayEvent的Tag */
	UPROPERTY(EditAnywhere, Category = "WeaponAttack")
	FGameplayTag EventTag;

	/** 碰撞检测成功后发送的PayLoad的Tag */
	UPROPERTY(EditAnywhere, Category = "WeaponAttack")
	FGameplayTag PayLoadTag;
	
	/** 是否对友方造成伤害 */
	UPROPERTY(EditAnywhere, Category = "WeaponAttack")
	bool bCanDamageToFriendlyActor;
	
	/** 是否进行tick更新检测碰撞 */
	UPROPERTY(EditAnywhere, Category = "WeaponAttack")
	bool bUseTickCheck;
	
	/** 是否使用自定义的碰撞形状 */
	UPROPERTY(EditAnywhere, Category = "WeaponAttack")
	bool bUseCustomDefinedCollision;

	/** 碰撞检测通道 */
	UPROPERTY(EditAnywhere, Category = "WeaponAttack")
	TArray<TEnumAsByte<ECollisionChannel>> TraceChannels;

	UPROPERTY(EditAnywhere, Category = "WeaponAttack : CustomCollision", meta = (EditCondition = "bUseCustomDefinedCollision"))
	TArray<FWeaponCustomCollisionDef> CustomCollisionDefs;

	/** 配置攻击方向  （关系到受击角色播放受击动画的方向和受击特效方向）*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator AttackRotationOffset;

private:
	USkeletalMeshComponent* GetWeaponMeshComp(USkeletalMeshComponent* MeshComp);
	void SetWeaponMeshComp(USkeletalMeshComponent* MeshComp, USkeletalMeshComponent* InWeaponMeshComp);

private:
	TArray<FSkeletalMeshBoneCollisionDataCache> TraceShapeData;
	TArray<FCollisionShape> CustomCollisionShapes;
	TANSDataContainer<FANSHitActorsData> DataContainer;
	TANSDataContainer<bool> HaveSendBeginCheckEventTagContainer;
	
	UPROPERTY()
	TMap<TObjectPtr<USkeletalMeshComponent>, FWeaponAttackInfoDef> AttackInfoMap;
	
	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComp;

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> OwnerMeshComp;
};
