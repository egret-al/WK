// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/ActorComponent.h"
#include "WKSkillComponent.generated.h"


class UWKAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLACKWK_API UWKSkillComponent : public UActorComponent, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UWKSkillComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure)
	UWKAbilitySystemComponent* GetWKAbilitySystemComponent() const;
	
	/** 检查技能是否在冷却中 */
	UFUNCTION(BlueprintPure, Category = "Skill|Cooldown")
	bool IsSkillInCooldown(const int32 SkillID) const;

	/** 获取剩余冷却时间 */
	UFUNCTION(BlueprintPure, Category = "Skill|Cooldown")
	bool GetSkillCooldown(const int32 SkillID, FDateTime& OutCooldown) const;

	/** 开始一个技能冷却 */
	UFUNCTION(BlueprintCallable, Category = "Skill|Cooldown")
	void StartSkillCooldown(const int32 SkillID);

	/** 手动清除冷却 */
	UFUNCTION(BlueprintCallable, Category="Skill|Cooldown")
	void ClearSkillCooldown(const int32 SkillID);

protected:
	// 记录技能CD恢复的时间
	TMap<int32, FDateTime> SkillCooldownMap;
};
