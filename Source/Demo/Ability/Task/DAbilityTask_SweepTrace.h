// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "DAbilityTask_SweepTrace.generated.h"

class ISweepInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerSyncTargetDataBySweepDelegate, const FGameplayAbilityTargetDataHandle&, Data);

UCLASS()
class DEMO_API UDAbilityTask_SweepTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	UDAbilityTask_SweepTrace();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "DAbility|Task", meta = (DisplayName = "ServerSyncTargetDataBySweep",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = true))
	static UDAbilityTask_SweepTrace* CreateSweepTrace(UGameplayAbility* OwningAbility);

	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void StartLogic(bool bInitialOverlap);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void StopLogic();
	
	UPROPERTY(BlueprintAssignable)
	FServerSyncTargetDataBySweepDelegate ValidData;

protected:
	virtual void Activate() override;

	void TriggerOnSwept(TArray<FHitResult>& HitResults) const;
	
private:
	UPROPERTY()
	TWeakObjectPtr<UPrimitiveComponent> SweptComponent;

	UPROPERTY()
	TArray<FHitResult> SweepResults;
	
	UPROPERTY()
	FVector LastLocation = FVector::ZeroVector;

	UPROPERTY()
	uint8 bCanTrace : 1 = false;

	FComponentQueryParams QueryParams;

	FDelegateHandle DelegateHandle;
};