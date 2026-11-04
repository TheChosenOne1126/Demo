// Fill out your copyright notice in the Description page of Project Settings.

#include "DAbilitySystemComponent.h"
#include "Animation/DAnimInstance.h"
#include "DataAsset/AbilityDataAsset.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"
#include "Player/DPlayerState.h"

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

void UDAbilitySystemComponent::OnPlayerControllerSet()
{
	Super::OnPlayerControllerSet();
	
	if (PlayerControllerSet.IsBound() && AbilityActorInfo.IsValid())
	{
		PlayerControllerSet.Broadcast(AbilityActorInfo->PlayerController.Get());
	}
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
	const FGameplayAbilitySpecHandle& Handle,
	const FPredictionKey& AbilityOriginalPredictionKey,
	const FPredictionKey& CurrentPredictionKey,
	const FGameplayTag& PayloadTag)
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
	const FGameplayAbilitySpecHandle& Handle,
	const FPredictionKey& AbilityOriginalPredictionKey,
	const FPredictionKey& CurrentPredictionKey,
	const FGameplayTag& PayloadTag)
{
	FScopedPredictionWindow Scoped(this, CurrentPredictionKey);
	
	InvokeReplicatedPayload(Tag, Handle, AbilityOriginalPredictionKey, CurrentPredictionKey, PayloadTag);
}

bool UDAbilitySystemComponent::ServerSetReplicatedPayload_Validate(
	const FGameplayTag& Tag,
	const FGameplayAbilitySpecHandle& Handle,
	const FPredictionKey& AbilityOriginalPredictionKey,
	const FPredictionKey& CurrentPredictionKey,
	const FGameplayTag& PayloadTag)
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
	
}

void UDAbilitySystemComponent::AbilityForInputStart(FGameplayTag InputTag)
{
	
}

void UDAbilitySystemComponent::AbilityForInputTrigger(FGameplayTag InputTag)
{
	if (!InputTag.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid InputTag"), __FUNCTION__));
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : CachePressedHandles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!Spec)
		{
			continue;
		}
	}
}

void UDAbilitySystemComponent::AbilityForInputCancel(FGameplayTag InputTag)
{
	if (!InputTag.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid InputTag"), __FUNCTION__));
		return;
	}
}

void UDAbilitySystemComponent::GiveAbilityWithData(TSubclassOf<UGameplayAbility> AbilityClass, const FAbilityData& Data)
{
	if (!IsValid(AbilityClass))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilityClass"), __FUNCTION__));
		return;
	}
	
	FGameplayAbilitySpec Spec(AbilityClass, Data.InitialAbilityLevel);
	Spec.GetDynamicSpecSourceTags().AppendTags(Data.Tags);
	Spec.SetByCallerTagMagnitudes.Emplace(GlobalTags::SetByCallerAbilityMaxLevelTag, Data.MaxAbilityLevel);
	GiveAbility(Spec);
}

void UDAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	const ADPlayerState* PS = Cast<ADPlayerState>(GetOwner());
	if (!IsValid(PS))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid ADPlayerState"), __FUNCTION__));
		return;
	}
	
	if (!PS->GetOwningController()->IsLocalPlayerController())
	{
		return;
	}
	
	if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityActivationInputPressedTag))
	{
		CachePressedHandles.Add(AbilitySpec.Handle);
	}
	else if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityActivationInputHeldTag))
	{
		CacheHeldHandles.Add(AbilitySpec.Handle);
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
