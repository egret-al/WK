// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAbilityTask_WaitGameplayTags.h"

#include "AbilitySystemComponent.h"

UWKAbilityTask_WaitGameplayTags::UWKAbilityTask_WaitGameplayTags(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivateCheck = true;
	bActivateCheckOnlyCallAdd = false;
}

void UWKAbilityTask_WaitGameplayTags::Activate()
{
	if (AbilitySystemComponent.IsValid())
	{
		DelegateHandles.Reserve(TagContainer.Num());
		WaitTags.Reserve(TagContainer.Num());

		int32 RemovedCounter = 0;
		for (const FGameplayTag& Tag : TagContainer)
		{
			DelegateHandles.Add(Tag, AbilitySystemComponent->RegisterGameplayTagEvent(Tag).AddUObject(this, &ThisClass::GameplayTagCallback));
			
			if (ActivateCheck && ShouldBroadcastAbilityTaskDelegates())
			{
				if (AbilitySystemComponent->HasMatchingGameplayTag(Tag))
				{
					WaitTags.AddUnique(Tag);
					Added.Broadcast(Tag);
				}
				else if(!bActivateCheckOnlyCallAdd)
				{
					Removed.Broadcast(Tag);
					RemovedCounter++;
				}
			}
		}
		
		if (ActivateCheck && RemovedCounter >= DelegateHandles.Num() && !bActivateCheckOnlyCallAdd)
		{
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				AllRemoved.Broadcast(FGameplayTag());
			}
		}
	}
}

void UWKAbilityTask_WaitGameplayTags::OnDestroy(bool AbilityIsEnding)
{
	if (AbilitySystemComponent.IsValid())
	{
		for (const FGameplayTag& Tag : TagContainer)
		{
			if (FDelegateHandle* DelegateHandle = DelegateHandles.Find(Tag))
			{
				AbilitySystemComponent->RegisterGameplayTagEvent(Tag).Remove(*DelegateHandle);
			}
		}

		WaitTags.Empty();
		DelegateHandles.Empty();
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UWKAbilityTask_WaitGameplayTags::GameplayTagCallback(const FGameplayTag InTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			WaitTags.Remove(InTag);

			if (WaitTags.Num() == 0)
			{
				AllRemoved.Broadcast(FGameplayTag());
			}

			Removed.Broadcast(InTag);
		}
	}
	else if (NewCount == 1)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			WaitTags.AddUnique(InTag);
			Added.Broadcast(InTag);
		}
	}
}

UWKAbilityTask_WaitGameplayTags* UWKAbilityTask_WaitGameplayTags::WaitGameplayTags(UGameplayAbility* OwningAbility, FGameplayTagContainer Container, bool ActivateCheck, bool bActivateCheckOnlyCallAdd)
{
	UWKAbilityTask_WaitGameplayTags* MyObj = NewAbilityTask<UWKAbilityTask_WaitGameplayTags>(OwningAbility);
	MyObj->TagContainer = Container;
	MyObj->ActivateCheck = ActivateCheck;
	MyObj->bActivateCheckOnlyCallAdd = bActivateCheckOnlyCallAdd;
	return MyObj;
}
