// Fill out your copyright notice in the Description page of Project Settings.

#include "DAbilitySystemComponent.h"
#include "Animation/DAnimInstance.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"
#include "Player/DCharacter.h"
#include "Player/DPlayerState.h"

void UDAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	// Check the owner actor is subclass of ADPlayerState
	if (!IsValid(Cast<ADPlayerState>(InOwnerActor)))
	{
		return;
	}

	// Check the avatar actor is subclass of ADCharacter
	if (!IsValid(Cast<ADCharacter>(InAvatarActor)))
	{
		return;
	}
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	UDAnimInstance* AnimInstance = Cast<UDAnimInstance>(AbilityActorInfo->GetAnimInstance());
	if (!IsValid(AnimInstance))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UDAnimInstance"), __FUNCTION__));
		return;
	}

	AnimInstance->InitializeTagsByAbilitySystem(this);
}

void UDAbilitySystemComponent::AbilityForInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputTag is invalid"), __FUNCTION__));
		return;
	}
	
	FGameplayEventData EventData;
	EventData.EventTag = InputTag;
	HandleGameplayEvent(GlobalTags::AbilityInputPressTag, &EventData);
	
	ABILITYLIST_SCOPE_LOCK()
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		const FGameplayTagContainer& SourceTags = AbilitySpec.GetDynamicSpecSourceTags();

		if (!SourceTags.HasTagExact(InputTag))
		{
			continue;
		}

		if (!IsValid(AbilitySpec.Ability))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilityCDO, InputTag[%s]"),
				__FUNCTION__, *InputTag.ToString()));
			continue;
		}

		if (SourceTags.HasTagExact(GlobalTags::AbilityActivationInputPressedTag))
		{
			InputPressedHandles.AddUnique(AbilitySpec.Handle);
		}
		else if (SourceTags.HasTagExact(GlobalTags::AbilityActivationInputHeldTag))
		{
			InputHeldHandles.AddUnique(AbilitySpec.Handle);
		}
		else
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Fatal Ability Activation Tag, InputTag[%s]"),
				__FUNCTION__, *InputTag.ToString()));
		}
	}
}

void UDAbilitySystemComponent::AbilityForInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputTag is invalid"), __FUNCTION__));
		return;
	}
	
	FGameplayEventData EventData;
	EventData.EventTag = InputTag;
	HandleGameplayEvent(GlobalTags::AbilityInputReleaseTag, &EventData);

	ABILITYLIST_SCOPE_LOCK()
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			if (!IsValid(AbilitySpec.Ability))
			{
				UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilityCDO InputTag[%s]"),
					__FUNCTION__, *InputTag.ToString()));
				continue;
			}
			
			InputReleasedHandles.AddUnique(AbilitySpec.Handle);
			InputHeldHandles.Remove(AbilitySpec.Handle);
		}
	}
}

void UDAbilitySystemComponent::ProcessAbilityInput()
{
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandlesToActivate;
	
	for (const FGameplayAbilitySpecHandle& Handle : InputHeldHandles)
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
		if (!AbilitySpec)
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Held AbilitySpec"), __FUNCTION__));
			continue;
		}

		if (!IsValid(AbilitySpec->Ability))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Held AbilityCDO"), __FUNCTION__));
			continue;
		}

		AbilitySpecInputPressed(*AbilitySpec);
		
		if (!AbilitySpec->IsActive())
		{
			AbilitySpecHandlesToActivate.AddUnique(AbilitySpec->Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& Handle : InputPressedHandles)
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
		if (!AbilitySpec)
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Pressed AbilitySpec"), __FUNCTION__));
			continue;
		}
		
		AbilitySpecInputPressed(*AbilitySpec);

		if (!AbilitySpec->IsActive())
		{
			AbilitySpecHandlesToActivate.AddUnique(AbilitySpec->Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandlesToActivate)
	{
		TryActivateAbility(Handle);
	}

	for (const FGameplayAbilitySpecHandle& Handle : InputReleasedHandles)
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
		if (!AbilitySpec)
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Released AbilitySpec"), __FUNCTION__));
			continue;
		}

		AbilitySpecInputReleased(*AbilitySpec);
	}

	InputPressedHandles.Reset();
	InputReleasedHandles.Reset();
}

void UDAbilitySystemComponent::ServerHandleGameplayEvent_Implementation(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	HandleGameplayEvent(EventTag, &Payload);
}

bool UDAbilitySystemComponent::ServerHandleGameplayEvent_Validate(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	return true;
}

void UDAbilitySystemComponent::UnregisterAllAttributeValuesChange()
{
	for (auto&[Attribute, Handle] : AttributeValueChangeDelegateMap)
	{
		GetGameplayAttributeValueChangeDelegate(Attribute).Remove(Handle);
	}
}
