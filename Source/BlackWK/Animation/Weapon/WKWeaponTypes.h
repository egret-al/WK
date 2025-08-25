// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

struct FWKWeaponConsoleVariables
{
	/** 绘制碰撞形状的时间 */
	static TAutoConsoleVariable<float> CVarDebugMeleeCollision;
};
