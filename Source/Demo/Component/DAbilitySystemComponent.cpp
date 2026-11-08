// Fill out your copyright notice in the Description page of Project Settings.

#include "DAbilitySystemComponent.h"
#include "Animation/DAnimInstance.h"
#include "DataAsset/AbilityDataAsset.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"

void UDAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	UDAnimInstance* AnimInstance = Cast<UDAnimInstance>(AbilityActorInfo->GetAnimInstance());
	if (!IsValid(AnimInstance))
	{
		//UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UDAnimInstance"), __FUNCTION__));
		return;
	}

	AnimInstance->InitializeTagsByAbilitySystem(this);
}

FString UDAbilitySystemComponent::GetModuleName_Implementation() const
{
	return TEXT("Component.AbilitySystemComponent");
}

void UDAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	if (IsValid(Ability))
	{
		if (Ability->IsInstantiated())
		{
			const FGameplayAbilityActivationInfo ActivationInfo = Ability->GetCurrentActivationInfo();
			AbilityReplicatedPayloadContainer.Remove(FGameplayAbilitySpecHandleAndPredictionKey(Handle, ActivationInfo.GetActivationPredictionKey()));
		}
	}
	else
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
	}
	
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
}

void UDAbilitySystemComponent::ConsumeReplicatedPayload(const FGameplayTag& Tag, const FGameplayAbilitySpecHandle& Handle, const FPredictionKey& Key) const
{
	const TSharedPtr<FAbilityReplicatedPayloadCache> Cache = AbilityReplicatedPayloadContainer.Find(FGameplayAbilitySpecHandleAndPredictionKey(Handle, Key));
	if (!Cache.IsValid())
	{
		return;
	}
	
	if (FAbilityReplicatedPayload* Data = Cache->TagEvents.Find(Tag))
	{
		Data->bTriggered = false;
	}
}

FGameplayTag UDAbilitySystemComponent::GetReplicatedPayloadTag(
	const FGameplayTag& Tag,
	const FGameplayAbilitySpecHandle& Handle,
	const FPredictionKey& Key) const
{
	const TSharedPtr<FAbilityReplicatedPayloadCache> Cache = AbilityReplicatedPayloadContainer.Find(
		FGameplayAbilitySpecHandleAndPredictionKey(Handle, Key));
	
	if (Cache.IsValid())
	{
		if (const FAbilityReplicatedPayload* Payload = Cache->TagEvents.Find(Tag))
		{
			return Payload->PayloadTag;
		}
	}
	
	return FGameplayTag::EmptyTag;
}

void UDAbilitySystemComponent::InvokeReplicatedPayload(
	const FGameplayTag& Tag,
	const FGameplayTag& PayloadTag,
	const FGameplayAbilitySpecHandle& Handle,
	const FPredictionKey& AbilityOriginalPredictionKey,
	const FPredictionKey& CurrentPredictionKey)
{
	const TSharedRef<FAbilityReplicatedPayloadCache> Cache = AbilityReplicatedPayloadContainer.FindOrAdd(
		FGameplayAbilitySpecHandleAndPredictionKey(Handle, AbilityOriginalPredictionKey));
	
	Cache->PredictionKey = CurrentPredictionKey;
	if (FAbilityReplicatedPayload* Data = Cache->TagEvents.Find(Tag))
	{
		Data->bTriggered = true;
		Data->PayloadTag = PayloadTag;
		if (Data->Delegate.IsBound())
		{
			Data->Delegate.Broadcast();
		}
	}
}

void UDAbilitySystemComponent::ServerSetReplicatedPayload_Implementation(
	const FGameplayTag& Tag,
	const FGameplayTag& PayloadTag,
	const FGameplayAbilitySpecHandle& Handle,
	const FPredictionKey& AbilityOriginalPredictionKey,
	const FPredictionKey& CurrentPredictionKey)
{
	FScopedPredictionWindow Scoped(this, CurrentPredictionKey);
	
	InvokeReplicatedPayload(Tag, PayloadTag, Handle, AbilityOriginalPredictionKey, CurrentPredictionKey);
}

bool UDAbilitySystemComponent::ServerSetReplicatedPayload_Validate(
	const FGameplayTag& Tag,
	const FGameplayTag& PayloadTag,
	const FGameplayAbilitySpecHandle& Handle,
	const FPredictionKey& AbilityOriginalPredictionKey,
	const FPredictionKey& CurrentPredictionKey)
{
	return true;
}

bool UDAbilitySystemComponent::CallReplicatedEventDelegateIfSet(
	const FGameplayTag& Tag,
	const FGameplayAbilitySpecHandle& Handle,
	const FPredictionKey& Key) const
{
	const TSharedPtr<FAbilityReplicatedPayloadCache> Cache = AbilityReplicatedPayloadContainer.Find(
		FGameplayAbilitySpecHandleAndPredictionKey(Handle, Key));

	if (Cache.IsValid())
	{
		if (const FAbilityReplicatedPayload* Data = Cache->TagEvents.Find(Tag))
		{
			if (Data->bTriggered)
			{
				Data->Delegate.Broadcast();
				return true;
			}
		}
	}
	
	return false;
}

FSimpleMulticastDelegate& UDAbilitySystemComponent::AbilityReplicatedPayloadDelegate(
	const FGameplayTag& Tag,
	const FGameplayAbilitySpecHandle& Handle,
	const FPredictionKey& Key)
{
	const TSharedRef<FAbilityReplicatedPayloadCache> Cache = AbilityReplicatedPayloadContainer.FindOrAdd(
		FGameplayAbilitySpecHandleAndPredictionKey(Handle, Key));

	FAbilityReplicatedPayload* Payload = Cache->TagEvents.Find(Tag);
	checkf(Payload, TEXT("Invalid Replicated Payload, Tag: [%s]"), *Tag.ToString());
	
	return Payload->Delegate;
}

void UDAbilitySystemComponent::ProcessAbilityInput()
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();
	
	for (auto& [Handle, Tag] : InputStartHandleToTagMap)
	{
		const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!Spec)
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputStart invalid AbilitySpec"), __FUNCTION__));
			return;
		}
		
		if (Spec->IsActive())
		{
			const UGameplayAbility* Ability = Spec->GetPrimaryInstance();
			if (!IsValid(Ability))
			{
				UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputStart invalid Ability"), __FUNCTION__));
				return;
			}
		
			const FPredictionKey Key = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();
			InvokeReplicatedPayload(GlobalTags::GA_INPUT_START, Tag, Handle, Key);
		}
		else
		{
			AbilitiesToActivate.Emplace(Handle);
		}
	}

	for (auto& [Handle, Tag] : InputTriggerHandleToTagMap)
	{
		const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!Spec)
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputTrigger invalid AbilitySpec"), __FUNCTION__));
			return;
		}
		
		if (Spec->IsActive())
		{
			const UGameplayAbility* Ability = Spec->GetPrimaryInstance();
			if (!IsValid(Ability))
			{
				UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputTrigger invalid Ability"), __FUNCTION__));
				return;
			}
		
			const FPredictionKey Key = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();
			InvokeReplicatedPayload(GlobalTags::GA_INPUT_TRIGGER, Tag, Handle, Key);
		}
		else
		{
			AbilitiesToActivate.Emplace(Handle);
		}
	}

	for (FGameplayAbilitySpecHandle Handle : AbilitiesToActivate)
	{
		TryActivateAbility(Handle);
	}

	for (auto& [Handle, Tag] : InputCancelHandleToTagMap)
	{
		const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!Spec)
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputCancel invalid AbilitySpec"), __FUNCTION__));
			return;
		}
		
		if (Spec->IsActive())
		{
			const UGameplayAbility* Ability = Spec->GetPrimaryInstance();
			if (!IsValid(Ability))
			{
				UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputCancel invalid Ability"), __FUNCTION__));
				return;
			}
		
			const FPredictionKey Key = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();
			InvokeReplicatedPayload(GlobalTags::GA_INPUT_CANCEL, Tag, Handle, Key);
		}
	}
	
	InputStartHandleToTagMap.Reset();
	InputTriggerHandleToTagMap.Reset();
	InputCancelHandleToTagMap.Reset();
}

void UDAbilitySystemComponent::AbilityForInputStart(const FGameplayTag& InputTag)
{
	ABILITYLIST_SCOPE_LOCK()
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (!IsValid(Spec.Ability))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(
				TEXT("%hs: invalid Ability, InputTag: [%s]"), __FUNCTION__, *InputTag.ToString()));
			continue;
		}

		if (!Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}
		
		InputStartHandleToTagMap.Emplace(Spec.Handle, InputTag);
	}
}

void UDAbilitySystemComponent::AbilityForInputTrigger(const FGameplayTag& InputTag)
{
	ABILITYLIST_SCOPE_LOCK()
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (!IsValid(Spec.Ability))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(
				TEXT("%hs: invalid Ability, InputTag: [%s]"), __FUNCTION__, *InputTag.ToString()));
			continue;
		}

		if (!Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}
		
		InputTriggerHandleToTagMap.Emplace(Spec.Handle, InputTag);
	}
}

void UDAbilitySystemComponent::AbilityForInputCancel(const FGameplayTag& InputTag)
{
	ABILITYLIST_SCOPE_LOCK()
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (!IsValid(Spec.Ability))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(
				TEXT("%hs: invalid Ability, InputTag: [%s]"), __FUNCTION__, *InputTag.ToString()));
			continue;
		}

		if (!Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}
		
		InputCancelHandleToTagMap.Emplace(Spec.Handle, InputTag);
	}
}

void UDAbilitySystemComponent::InternalServerTryActivateAbility(
	FGameplayAbilitySpecHandle Handle,
	bool InputPressed,
	const FPredictionKey& PredictionKey,
	const FGameplayEventData* TriggerEventData)
{
	const TSharedPtr<FAbilityReplicatedPayloadCache> Cache = AbilityReplicatedPayloadContainer.Find(
		FGameplayAbilitySpecHandleAndPredictionKey(Handle, PredictionKey));
	
	if (Cache.IsValid())
	{
		Cache->Reset();
	}
	
	Super::InternalServerTryActivateAbility(Handle, InputPressed, PredictionKey, TriggerEventData);
}
