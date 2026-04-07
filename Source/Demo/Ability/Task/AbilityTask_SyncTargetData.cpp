// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityTask_SyncTargetData.h"
#include "AbilitySystemComponent.h"
#include "Global/Statics.h"

UAbilityTask_SyncTargetData* UAbilityTask_SyncTargetData::SyncTargetData(UGameplayAbility* OwningAbility, bool bOnlySyncOnce)
{
	ThisClass* AbilityTask = NewAbilityTask<ThisClass>(OwningAbility);
	AbilityTask->bOnlySyncOnce = bOnlySyncOnce;
	return AbilityTask;
}

void UAbilityTask_SyncTargetData::CallServerSetTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ApplicationTag) const
{
	if (!IsPredictingClient())
	{
		return;
	}
	
	if (!AbilitySystemComponent.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		return;
	}
	
	const FGameplayAbilitySpecHandle& Handle = GetAbilitySpecHandle();
	const FPredictionKey& Key = GetActivationPredictionKey();

	if (!Handle.IsValid() || !Key.IsValidKey())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Handle or Key"), __FUNCTION__));
		return;
	}
	
	AbilitySystemComponent->CallServerSetReplicatedTargetData(Handle, Key, Data, ApplicationTag, AbilitySystemComponent->ScopedPredictionKey);
}

void UAbilityTask_SyncTargetData::OnDestroy(bool bInOwnerFinished)
{
	if (IsForRemoteClient())
	{
		if (!AbilitySystemComponent.IsValid())
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
			return;
		}

		const FGameplayAbilitySpecHandle& Handle = GetAbilitySpecHandle();
		const FPredictionKey& Key = GetActivationPredictionKey();

		if (!Handle.IsValid() || !Key.IsValidKey())
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Handle or Key"), __FUNCTION__));
			return;
		}
		AbilitySystemComponent->AbilityTargetDataSetDelegate(Handle, Key).RemoveAll(this);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_SyncTargetData::Activate()
{
	Super::Activate();

	if (IsForRemoteClient())
	{
		if (!AbilitySystemComponent.IsValid())
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
			return;
		}

		const FGameplayAbilitySpecHandle& Handle = GetAbilitySpecHandle();
		const FPredictionKey& Key = GetActivationPredictionKey();

		if (!Handle.IsValid() || !Key.IsValidKey())
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Handle or Key"), __FUNCTION__));
			return;
		}
		
		AbilitySystemComponent->AbilityTargetDataSetDelegate(Handle, Key).AddUObject(this, &ThisClass::OnTargetDataSet);
		if (!AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(Handle, Key))
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UAbilityTask_SyncTargetData::OnTargetDataSet(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag Tag)
{
	if (!AbilitySystemComponent.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		return;
	}
	
	const FGameplayAbilitySpecHandle& Handle = GetAbilitySpecHandle();
	const FPredictionKey& Key = GetActivationPredictionKey();

	if (!Handle.IsValid() || !Key.IsValidKey())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Handle or Key"), __FUNCTION__));
		return;
	}
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(Handle, Key);
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		SyncData.Broadcast(Data, Tag);
	}

	if (bOnlySyncOnce)
	{
		EndTask();
	}
}
