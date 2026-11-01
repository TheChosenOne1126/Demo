// Fill out your copyright notice in the Description page of Project Settings.

#include "DAbilityTypes.h"

TSharedPtr<FDAbilityReplicatedDataCache> FDAbilityReplicatedDataContainer::Find(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey) const
{
	for (auto& [HandleAndKey, DataCache] : InUseData)
	{
		if (InHandleAndKey == HandleAndKey)
		{
			return DataCache;
		}
	}
	
	return TSharedPtr<FDAbilityReplicatedDataCache>();
}

TSharedRef<FDAbilityReplicatedDataCache> FDAbilityReplicatedDataContainer::FindOrAdd(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey)
{
	TSharedPtr<FDAbilityReplicatedDataCache> SharedPtr = Find(InHandleAndKey);
	if (SharedPtr.IsValid())
	{
		return SharedPtr.ToSharedRef();
	}
	
	for (int32 i = FreeData.Num()-1; i >= 0; --i)
	{
		TSharedRef<FDAbilityReplicatedDataCache>& FreeRef = FreeData[i];
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
		SharedPtr = MakeShared<FDAbilityReplicatedDataCache>();
	}

	TSharedRef<FDAbilityReplicatedDataCache> SharedRef = SharedPtr.ToSharedRef();
	InUseData.Emplace(InHandleAndKey, SharedRef);
	return SharedRef;
}

void FDAbilityReplicatedDataContainer::Remove(const FGameplayAbilitySpecHandleAndPredictionKey& InHandleAndKey)
{
}

void FDAbilityReplicatedDataContainer::PrintDebug()
{
}
