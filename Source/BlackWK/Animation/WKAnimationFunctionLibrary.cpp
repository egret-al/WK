// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimationFunctionLibrary.h"

#include "Animation/AnimNotifies/AnimNotifyState.h"

void SortNotifyArray(TArray<const FAnimNotifyEvent*>& NotifyArr)
{
	if (NotifyArr.Num() > 2)
	{
		int low = 0;
		int high = NotifyArr.Num() - 1; 
		const FAnimNotifyEvent* tmp;
		while (low < high)
		{
			for (int j = low; j < high; ++j) 
			{
				if (NotifyArr[j]->GetTriggerTime() > NotifyArr[j + 1]->GetTriggerTime())
				{
					tmp = NotifyArr[j];
					NotifyArr[j] = NotifyArr[j + 1];
					NotifyArr[j + 1] = tmp;
				}
			}
			--high;                

			for (int j = high; j > low; --j) 
			{
				if (NotifyArr[j] < NotifyArr[j - 1])
				{
					tmp = NotifyArr[j];
					NotifyArr[j] = NotifyArr[j - 1];
					NotifyArr[j - 1] = tmp;
				}
			}
			++low;              
		}
	}
}

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

TArray<const FAnimNotifyEvent*> UWKAnimationFunctionLibrary::GetNotifyEventsFromAnimMontage(const UAnimMontage* AnimMontage, const UClass* NotifyClass, const int32 SectionIndex)
{
	TArray<const FAnimNotifyEvent*> OutNotifyEvents;
	if (AnimMontage == nullptr)
	{
		return OutNotifyEvents;
	}

	float StartTime = 0;
	float StopTime = AnimMontage->GetPlayLength();
	if (SectionIndex != INDEX_NONE)
	{
		AnimMontage->GetSectionStartAndEndTime(SectionIndex, StartTime, StopTime);
	}
	
	FAnimTickRecord TickRecord;
	TickRecord.bLooping = false;
	FAnimNotifyContext NotifyContext(TickRecord);
	AnimMontage->GetAnimNotifies(StartTime, StopTime - StartTime, NotifyContext);

	for (int i = 0; i < NotifyContext.ActiveNotifies.Num(); i++)
	{
		if (const FAnimNotifyEvent* NotifyEvent = NotifyContext.ActiveNotifies[i].GetNotify())
		{
			if (NotifyClass->IsChildOf(UAnimNotify::StaticClass()))
			{
				if (NotifyEvent->Notify && NotifyEvent->Notify->GetClass()->IsChildOf(NotifyClass))
				{
					OutNotifyEvents.Add(NotifyEvent);
				}
			}
			else if (NotifyClass->IsChildOf(UAnimNotifyState::StaticClass()))
			{
				if (NotifyEvent->NotifyStateClass && NotifyEvent->NotifyStateClass->GetClass()->IsChildOf(NotifyClass))
				{
					OutNotifyEvents.Add(NotifyEvent);
				}
			}
		}
	}

	SortNotifyArray(OutNotifyEvents);
	return OutNotifyEvents;
}
