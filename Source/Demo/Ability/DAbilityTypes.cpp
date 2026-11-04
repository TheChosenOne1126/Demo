// Fill out your copyright notice in the Description page of Project Settings.

#include "DAbilityTypes.h"

TSharedPtr<FAbilityReplicatedPayloadCache> FDAbilityReplicatedPayloadContainer::Find(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey) const
{
	for (auto& [HandleAndKey, DataCache] : InUseData)
	{
		if (InHandleAndKey == HandleAndKey)
		{
			return DataCache;
		}
	}
	
	return TSharedPtr<FAbilityReplicatedPayloadCache>();
}

TSharedRef<FAbilityReplicatedPayloadCache> FDAbilityReplicatedPayloadContainer::FindOrAdd(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey)
{
	TSharedPtr<FAbilityReplicatedPayloadCache> SharedPtr = Find(InHandleAndKey);
	if (SharedPtr.IsValid())
	{
		return SharedPtr.ToSharedRef();
	}
	
	for (int32 i = 0; i < FreeData.Num(); i++)
	{
		TSharedRef<FAbilityReplicatedPayloadCache>& FreeRef = FreeData[i];
		if (FreeRef.IsUnique())
		{
			SharedPtr = FreeRef;

			SharedPtr->ResetAll();
			FreeData.RemoveAtSwap(i, EAllowShrinking::No);
			break;
		}
	}

	if (!SharedPtr.IsValid())
	{
		SharedPtr = MakeShared<FAbilityReplicatedPayloadCache>();
	}

	TSharedRef<FAbilityReplicatedPayloadCache> SharedRef = SharedPtr.ToSharedRef();
	InUseData.Emplace(InHandleAndKey, SharedRef);
	return SharedRef;
}

void FDAbilityReplicatedPayloadContainer::Remove(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey)
{
	for (int32 i = 0; i < InUseData.Num(); ++i)
	{
		if (InHandleAndKey == InUseData[i].Key)
		{
			TSharedRef<FAbilityReplicatedPayloadCache>& RemovedRef = InUseData[i].Value;
			
			FreeData.Emplace(RemovedRef);
			
			InUseData.RemoveAtSwap(i, EAllowShrinking::No);
			break;
		}
	}
}