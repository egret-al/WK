// Fill out your copyright notice in the Description page of Project Settings.


#include "WKGameplayStatics.h"

bool UWKGameplayStatics::WorldIsGame(const UObject* WorldContextObj)
{
	if (WorldContextObj && WorldContextObj->GetWorld())
	{
		const EWorldType::Type WorldType = WorldContextObj->GetWorld()->WorldType;
		if (WorldType == EWorldType::PIE || WorldType == EWorldType::Game)
		{
			return true;
		}
	}

	return false;
}
