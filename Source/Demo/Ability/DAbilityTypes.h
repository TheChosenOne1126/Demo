// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayPrediction.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"

struct FDAbilityReplicatedData
{
	FDAbilityReplicatedData() : bTriggered(false), TagPayload(FGameplayTag::EmptyTag) {}

	bool bTriggered;

	FGameplayTag TagPayload;

	FSimpleMulticastDelegate Delegate;
};

struct FDAbilityReplicatedDataCache
{
	FPredictionKey PredictionKey;
	
	TMap<FGameplayTag, FDAbilityReplicatedData> TagEvents;
	
	FDAbilityReplicatedDataCache() {}
	
	virtual ~FDAbilityReplicatedDataCache() { }
	
	void Reset()
	{
		PredictionKey = FPredictionKey();
		for (auto& [Tag, Data] : TagEvents)
		{
			Data.bTriggered = false;
			Data.TagPayload = FGameplayTag::EmptyTag;
		}
	}

	void ResetAll()
	{
		Reset();
		for (auto& [Tag, Data] : TagEvents)
		{
			Data.Delegate.Clear();
		}
	}
};

struct FDAbilityReplicatedDataContainer
{
	TSharedPtr<FDAbilityReplicatedDataCache> Find(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey) const;
	
	TSharedRef<FDAbilityReplicatedDataCache> FindOrAdd(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey);

	void Remove(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey);
	
	void PrintDebug();

private:
	TArray<TPair<FGameplayAbilitySpecHandleAndPredictionKey, TSharedRef<FDAbilityReplicatedDataCache>>> InUseData;
	
	TArray<TSharedRef<FDAbilityReplicatedDataCache>> FreeData;
};
