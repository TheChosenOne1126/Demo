// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityTask_WaitReplicatedPayload.h"
#include "Component/DAbilitySystemComponent.h"

UAbilityTask_WaitReplicatedPayload* UAbilityTask_WaitReplicatedPayload::WaitReplicatedPayload(
	UGameplayAbility* OwningAbility,
	FGameplayTag Tag,
	bool bTriggerOnce)
{
	ThisClass* NewTask = NewAbilityTask<ThisClass>(OwningAbility);
	NewTask->Tag = Tag;
	NewTask->bTriggerOnce = bTriggerOnce;
	return NewTask;
}

void UAbilityTask_WaitReplicatedPayload::OnDestroy(bool bInOwnerFinished)
{
	UDAbilitySystemComponent* Asc = Cast<UDAbilitySystemComponent>(AbilitySystemComponent);
	if (IsValid(Asc))
	{
		Asc->AbilityReplicatedPayloadDelegate(Tag, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitReplicatedPayload::Activate()
{
	Super::Activate();

	UDAbilitySystemComponent* Asc = Cast<UDAbilitySystemComponent>(AbilitySystemComponent);
	if (IsValid(Asc))
	{
		DelegateHandle = Asc->AbilityReplicatedPayloadDelegate(Tag, GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(
			this, &ThisClass::OnTriggerPayload);

		if (IsForRemoteClient() && !Asc->CallReplicatedEventDelegateIfSet(Tag, GetAbilitySpecHandle(), GetActivationPredictionKey()))
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UAbilityTask_WaitReplicatedPayload::OnTriggerPayload()
{
	UDAbilitySystemComponent* Asc = Cast<UDAbilitySystemComponent>(AbilitySystemComponent);
	if (!IsValid(Asc))
	{
		return;
	}
	
	FScopedPredictionWindow ScopedPrediction(Asc, IsPredictingClient());

	const FGameplayTag PayloadTag = Asc->GetReplicatedPayloadTag(Tag, GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (IsPredictingClient())
	{
		Asc->ServerSetReplicatedPayload(Tag, GetAbilitySpecHandle(),
			GetActivationPredictionKey(), Asc->ScopedPredictionKey, PayloadTag);
	}
	else
	{
		Asc->ConsumeReplicatedPayload(Tag, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnReceived.Broadcast(PayloadTag);
	}

	if (bTriggerOnce)
	{
		EndTask();
	}
}
