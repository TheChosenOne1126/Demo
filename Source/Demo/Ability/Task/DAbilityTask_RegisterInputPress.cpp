// Fill out your copyright notice in the Description page of Project Settings.

#include "DAbilityTask_RegisterInputPress.h"
#include "AbilitySystemComponent.h"
#include "Global/Statics.h"

UDAbilityTask_RegisterInputPress* UDAbilityTask_RegisterInputPress::RegisterInputPress(UGameplayAbility* OwningAbility, bool bTriggerOnce)
{
	ThisClass* AbilityTask = NewAbilityTask<ThisClass>(OwningAbility);
	AbilityTask->bTriggerOnce = bTriggerOnce;
	AbilityTask->bCanTrigger = bTriggerOnce;
	return AbilityTask;
}

void UDAbilityTask_RegisterInputPress::SetEnableTrigger(bool bEnable)
{
	bCanTrigger = bEnable;
}

void UDAbilityTask_RegisterInputPress::Activate()
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
		if (Spec && Spec->InputPressed)
		{
			OnTriggerPressed();
			return;
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

void UDAbilityTask_RegisterInputPress::OnTriggerPressed()
{
	if (!AbilitySystemComponent.IsValid() || !bCanTrigger)
	{
		return;
	}

	AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed,
		GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);

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

	if (bTriggerOnce)
	{
		EndTask();
	}
}
