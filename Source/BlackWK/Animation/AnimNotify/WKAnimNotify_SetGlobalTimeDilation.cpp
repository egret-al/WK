// Fill out your copyright notice in the Description page of Project Settings.


#include "WKAnimNotify_SetGlobalTimeDilation.h"

#include "Kismet/GameplayStatics.h"

void UWKAnimNotify_SetGlobalTimeDilation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), GlobalTimeDilation);
}

FString UWKAnimNotify_SetGlobalTimeDilation::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("TimeDilation:%.2f"), GlobalTimeDilation);
}
