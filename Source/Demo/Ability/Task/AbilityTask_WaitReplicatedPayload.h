// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitReplicatedPayload.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReplicatedPayloadReceived, FGameplayTag, PayloadTag);

UCLASS()
class DEMO_API UAbilityTask_WaitReplicatedPayload : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="DAbility|Tasks",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_WaitReplicatedPayload* WaitReplicatedPayload(UGameplayAbility* OwningAbility, FGameplayTag Tag, bool bTriggerOnce);
	
	UPROPERTY(BlueprintAssignable)
	FReplicatedPayloadReceived OnReceived;
	
	virtual void OnDestroy(bool bInOwnerFinished) override;

protected:
	virtual void Activate() override;
	
	void OnTriggerPayload();

private:
	UPROPERTY()
	FGameplayTag Tag;
	
	UPROPERTY()
	bool bTriggerOnce;
	
	FDelegateHandle DelegateHandle;
};
