// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TickBoxOverlap.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTickBoxOverlapDelegate, bool, bIsOverlap, const TArray<AActor*>&, OverlapActors);

UCLASS()
class DEMO_API UAbilityTask_TickBoxOverlap : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UAbilityTask_TickBoxOverlap();
	
	UFUNCTION(meta = (AutoCreateRefTerm = "ActorsToIgnore"))
	static UAbilityTask_TickBoxOverlap* BoxOverlap(
		UGameplayAbility* OwningAbility,
		const FVector& BoxCenter,
		const FVector& BoxExtent,
		const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
		const TArray<AActor*>& ActorsToIgnore);
	
	virtual void TickTask(float DeltaTime) override;
	
	UFUNCTION()
	void UpdateBoxCenter(const FVector& InBoxCenter);
	
	UPROPERTY(BlueprintAssignable)
	FTickBoxOverlapDelegate Overlap;
	
private:
	UPROPERTY()
	FVector BoxExtent;
	
	UPROPERTY()
	FVector BoxCenter;
	
	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	
	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;
};