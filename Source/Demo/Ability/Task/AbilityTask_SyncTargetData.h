// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_SyncTargetData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSyncTargetDataDelegate, const FGameplayAbilityTargetDataHandle&, Data, FGameplayTag, Tag);

UCLASS()
class DEMO_API UAbilityTask_SyncTargetData : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_SyncTargetData* SyncTargetData(UGameplayAbility* OwningAbility, bool bOnlySyncOnce = false);
	
	UPROPERTY(BlueprintAssignable)
	FSyncTargetDataDelegate SyncData;
	
	UFUNCTION(BlueprintCallable)
	void CallServerSetTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ApplicationTag) const;
	
	virtual void OnDestroy(bool bInOwnerFinished) override;

protected:
	virtual void Activate() override;
	
	void OnTargetDataSet(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag Tag);
	
private:
	UPROPERTY()
	uint8 bOnlySyncOnce : 1;
};
