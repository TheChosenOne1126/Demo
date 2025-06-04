// Fill out your copyright notice in the Description page of Project Settings.

#include "DAbilityTask_ContinueToListenInputPress.h"
#include "AbilitySystemComponent.h"
#include "Global/Statics.h"

UDAbilityTask_ContinueToListenInputPress* UDAbilityTask_ContinueToListenInputPress::ContinueToListenInputPress(UGameplayAbility* OwningAbility, bool bStartTrigger)
{
	ThisClass* AbilityTask = NewAbilityTask<ThisClass>(OwningAbility);
	AbilityTask->bCanTrigger = bStartTrigger;
	return AbilityTask;
}

void UDAbilityTask_ContinueToListenInputPress::OnDestroy(bool bInOwnerFinished)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed,
			GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UDAbilityTask_ContinueToListenInputPress::SetEnableTrigger(bool bEnable)
{
	bCanTrigger = bEnable;
}

void UDAbilityTask_ContinueToListenInputPress::Activate()
{
	Super::Activate();

	if (!AbilitySystemComponent.IsValid() || !IsValid(Ability))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		return;
	}
	
	if (IsLocallyControlled())
	{
		const FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec();
		if (Spec && Spec->InputPressed && bCanTrigger)
		{
			OnTriggerPressed();
		}
	}

	DelegateHandle = AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed,
		GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTriggerPressed);
	
	if (IsForRemoteClient() && !AbilitySystemComponent->CallReplicatedEventDelegateIfSet(
		EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey()))
	{
		SetWaitingOnRemotePlayerData();
	}
}

void UDAbilityTask_ContinueToListenInputPress::OnTriggerPressed() const
{
	if (!AbilitySystemComponent.IsValid() || !bCanTrigger)
	{
		return;
	}

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());

	if (IsPredictingClient())
	{
		AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed,
			GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
	}
	else
	{
		AbilitySystemComponent->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnPress.Broadcast();
	}
}
