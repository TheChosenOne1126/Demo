// Fill out your copyright notice in the Description page of Project Settings.

#include "DAbilitySystemComponent.h"
#include "Animation/DAnimInstance.h"
#include "DataAsset/AbilityDataAsset.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"
#include "Player/DPlayerState.h"
#include "Player/HeroPlayerState.h"

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
	
	FGameplayEventData Data;
	Data.InstigatorTags.AddTag(InputTag);
	HandleGameplayEvent(GlobalTags::AbilityInputPressTag, &Data);
	
	ABILITYLIST_SCOPE_LOCK()
	for (FGameplayAbilitySpec& Spec: ActivatableAbilities.Items)
	{
		if (!Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag) || IsValid(Spec.Ability))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid InputTag: %s"), __FUNCTION__, *InputTag.ToString()));
			continue;
		}
		
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityActivationInputPressedTag))
		{
			InputTriggerHandles.Add(Spec.Handle);
		}
		else if (Spec.GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityActivationInputHeldTag))
		{
			InputStartHandles.Add(Spec.Handle);
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

	const AHeroPlayerState* PS = Cast<AHeroPlayerState>(GetOwner());
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
		CachePressedSpecs.Add(AbilitySpec);
	}
	else if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityActivationInputHeldTag))
	{
		CacheHeldSpecs.Add(AbilitySpec);
	}
}
