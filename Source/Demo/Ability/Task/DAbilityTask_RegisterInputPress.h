// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "DAbilityTask_RegisterInputPress.generated.h"

UCLASS()
class DEMO_API UDAbilityTask_RegisterInputPress : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DAbility|Task", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UDAbilityTask_RegisterInputPress* RegisterInputPress(UGameplayAbility* OwningAbility, bool bTriggerOnce = false);

	virtual void OnDestroy(bool bInOwnerFinished) override;

	UFUNCTION(BlueprintCallable)
	void SetEnableTrigger(bool bEnable);

protected:
	virtual void Activate() override;
	
	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnPress;

private:
	void OnTriggerPressed();
	
	UPROPERTY()
	uint8 bTriggerOnce : 1;

	UPROPERTY()
	uint8 bCanTrigger : 1;

	FDelegateHandle DelegateHandle;
};
