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

void UDAbilitySystemComponent::DestroyActiveState()
{
	if (!IsOwnerActorAuthoritative())
	{
		ABILITYLIST_SCOPE_LOCK()
		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			OnRemoveAbility(Spec);
		}
	}
	
	Super::DestroyActiveState();
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
