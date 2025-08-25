// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "WKAnimNotifyState.generated.h"

// 存放数据的Container，用于防止数据共享导致的覆盖
//由于TANSDataContainer使用了模板，不能弄成UClass，导致不能标UPROPERTY()来防止保存的对象被GC。所以TANSDataContainer里请不要使用原始指针，应该使用弱指针进行有效性判断
template<typename T>
class TANSDataContainer
{
public:
	T* Get(const USkeletalMeshComponent* MeshComponent)
	{
		if (auto Key = GenKey(MeshComponent); Key >= 0)
		{
			return DataContainer.Find(Key);
		}

		return nullptr;
	}
	
	T* GetOrAdd(const USkeletalMeshComponent* MeshComponent)
	{
		if (auto Key = GenKey(MeshComponent); Key >= 0)
		{
			return &DataContainer.FindOrAdd(Key);
		}

		return nullptr;
	}

	bool Remove(const USkeletalMeshComponent* MeshComponent)
	{
		if (auto Key = GenKey(MeshComponent); Key >= 0)
		{
			return DataContainer.Remove(Key) > 0;
		}

		return false;
	}

protected:
	static int GenKey(const USkeletalMeshComponent* MeshComponent)
	{
		if (MeshComponent)
		{
			return MeshComponent->GetUniqueID();
		}

		// return -1 as invalid key
		// NOTE key is valid when >= 0
		return -1;
	}

protected:
	TMap<int, T> DataContainer;
};

/**
 *
 *  注意： UAnimNotifyState不能有存储运行时数据的变量，否则 两个相同的角色播放同一个蒙太奇改动的是同一份数据 造成相互影响
 *  参考 https://blog.csdn.net/tkokof1/article/details/129024465
 *
 *  存储运行时数据的变量，请使用TANSDataContainer，或者存在角色DSAnimInstance或者其他角色自己的实例中
 *
 *  由于TANSDataContainer使用了模板，不能弄成UClass，导致不能标UPROPERTY()来防止保存的对象被GC。所以TANSDataContainer里请不要使用原始指针，应该使用弱指针进行有效性判断
 */
UCLASS()
class BLACKWK_API UWKAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
};
