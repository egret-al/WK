// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

struct FGameplayTag;

#define INDEX_INVALID (-1)
#define NAME_EMPTY ("None")

void AddNativeTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

// 便捷使用宏
#define SERIALIZE_BY_CONDITION(Condition, RepFunc, LoadDefaultValue)\
if (RepBits & (1 << Condition))\
{\
	RepFunc;\
}\
else if (Ar.IsLoading())\
{\
	LoadDefaultValue;\
}