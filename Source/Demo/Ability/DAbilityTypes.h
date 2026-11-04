// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayPrediction.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"

struct FAbilityReplicatedPayload
{
	FAbilityReplicatedPayload() : bTriggered(false), PayloadTag(FGameplayTag::EmptyTag) {}

	bool bTriggered;

	FGameplayTag PayloadTag;

	FSimpleMulticastDelegate Delegate;
	
	static FSimpleMulticastDelegate EmptyDelegate;
};

struct FAbilityReplicatedPayloadCache
{
	FPredictionKey PredictionKey;
	
	TMap<FGameplayTag, FAbilityReplicatedPayload> TagEvents;
	
	FAbilityReplicatedPayloadCache() {}
	
	virtual ~FAbilityReplicatedPayloadCache() { }
	
	void Reset()
	{
		PredictionKey = FPredictionKey();
		for (auto& [Tag, Data] : TagEvents)
		{
			Data.bTriggered = false;
			Data.PayloadTag = FGameplayTag::EmptyTag;
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

struct FDAbilityReplicatedPayloadContainer
{
	TSharedPtr<FAbilityReplicatedPayloadCache> Find(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey) const;
	
	TSharedRef<FAbilityReplicatedPayloadCache> FindOrAdd(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey);

	void Remove(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey);

private:
	TArray<TPair<FGameplayAbilitySpecHandleAndPredictionKey, TSharedRef<FAbilityReplicatedPayloadCache>>> InUseData;
	
	TArray<TSharedRef<FAbilityReplicatedPayloadCache>> FreeData;
};
