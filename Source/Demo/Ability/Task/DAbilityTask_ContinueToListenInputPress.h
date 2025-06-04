// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "DAbilityTask_ContinueToListenInputPress.generated.h"

UCLASS()
class DEMO_API UDAbilityTask_ContinueToListenInputPress : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DAbility|Task", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UDAbilityTask_ContinueToListenInputPress* ContinueToListenInputPress(UGameplayAbility* OwningAbility, bool bStartTrigger = false);

	virtual void OnDestroy(bool bInOwnerFinished) override;

	UFUNCTION(BlueprintCallable)
	void SetEnableTrigger(bool bEnable);

protected:
	virtual void Activate() override;
	
	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnPress;

private:
	void OnTriggerPressed() const;

	UPROPERTY()
	uint8 bCanTrigger : 1;

	FDelegateHandle DelegateHandle;
};
