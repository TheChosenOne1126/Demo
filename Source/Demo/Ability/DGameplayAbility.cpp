// Fill out your copyright notice in the Description page of Project Settings.

#include "DGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Global/GlobalTags.h"
#include "Global/MessageSubsystem.h"
#include "Global/Statics.h"

UDGameplayAbility::UDGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UDGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (IsLocallyControlled())
	{
		UMessageSubsystem* MessageSubsystem = UMessageSubsystem::Get(this);
		if (!IsValid(MessageSubsystem))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MessageSubsystem"), __FUNCTION__));
			return;
		}

		MessageSubsystem->BroadcastMessage(GlobalTags::AbilityOnGivenTag, Spec.Handle);
	}

	UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	if (IsValid(CooldownEffect) && CooldownTags.IsValid())
	{
		CooldownEffect->CachedGrantedTags.AppendTags(CooldownTags);
	}
}

void UDGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);

	UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	if (IsValid(CooldownEffect) && CooldownTags.IsValid())
	{
		CooldownEffect->CachedGrantedTags.RemoveTags(CooldownTags);
	}
}

void UDGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (!Spec.GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityActivationAutoTag))
	{
		return;
	}

	if (Spec.Level <= 0)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: AbilitySpec Level is less or equal than 0"), __FUNCTION__));
		return;
	}

	if (!ActorInfo)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilityActorInfo"), __FUNCTION__));
		return;
	}
	
	if (!ActorInfo->AbilitySystemComponent.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
}

bool UDGameplayAbility::CheckCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	return CooldownTags.IsEmpty() || Super::CheckCooldown(Handle, ActorInfo, OptionalRelevantTags);
}

void UDGameplayAbility::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	if (!IsValid(CooldownEffect) || CooldownTags.IsEmpty())
	{
		return;
	}

	const FGameplayEffectSpecHandle& EffectSpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CooldownEffect->GetClass());
	if (EffectSpecHandle.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid EffectSpecHandle"), __FUNCTION__));
		return;
	}
	
	EffectSpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);
	const float Magnitude = CooldownDuration.GetValueAtLevel(GetAbilityLevel(Handle, ActorInfo));
	EffectSpecHandle.Data->SetSetByCallerMagnitude(GlobalTags::SetByCallerCooldownValueTag, Magnitude);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
}

bool UDGameplayAbility::CheckCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	return SetByCallerCostMap.IsEmpty() || Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

void UDGameplayAbility::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const UGameplayEffect* CostEffect = GetCostGameplayEffect();
	if (!IsValid(CostEffect) || SetByCallerCostMap.IsEmpty())
	{
		return;
	}

	const FGameplayEffectSpecHandle& EffectSpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CostEffect->GetClass());
	if (EffectSpecHandle.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid EffectSpecHandle"), __FUNCTION__));
		return;
	}
	
	for (auto&[Tag, ScalableFloat] : SetByCallerCostMap)
	{
		const float Magnitude = ScalableFloat.GetValueAtLevel(GetAbilityLevel(Handle, ActorInfo));
		EffectSpecHandle.Data->SetSetByCallerMagnitude(Tag, Magnitude);
	}
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
}

#if WITH_EDITOR
void UDGameplayAbility::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, ActivationCancelTags))
	{
		ActivationBlockedTags.AppendTags(ActivationCancelTags);
	}
}
#endif

void UDGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilityActorInfo"), __FUNCTION__));
		return;
	}

	if (!ActorInfo->AbilitySystemComponent.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	if (!Handle.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilitySpecHandle"), __FUNCTION__));
		return;
	}
	
	FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (!AbilitySpec)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilitySpec"), __FUNCTION__));
		return;
	}

	ActorInfo->AbilitySystemComponent->AddLooseGameplayTags(AbilitySpec->GetDynamicSpecSourceTags());

	if (!HasAuthority(&ActivationInfo))
	{
		return;
	}

	auto Lambda = [this](const FGameplayTag Tag, int32 NewCount) -> void
	{
		if (NewCount > 0 && ActivationCancelTags.HasTagExact(Tag))
		{
			CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		}
	};

	for (const FGameplayTag& ActivationCancelTag : ActivationCancelTags)
	{
		ActorInfo->AbilitySystemComponent->RegisterGameplayTagEvent(ActivationCancelTag).AddWeakLambda(this, Lambda);
	}
}

void UDGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!ActorInfo)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilityActorInfo"), __FUNCTION__));
		return;
	}

	if (!ActorInfo->AbilitySystemComponent.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	if (!Handle.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilitySpecHandle"), __FUNCTION__));
		return;
	}
	
	FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (!AbilitySpec)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilitySpec"), __FUNCTION__));
		return;
	}

	ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTags(AbilitySpec->GetDynamicSpecSourceTags());
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	for (const FGameplayTag& ActivationCancelTag : ActivationCancelTags)
	{
		ActorInfo->AbilitySystemComponent->RegisterGameplayTagEvent(ActivationCancelTag).RemoveAll(this);
	}
}

UGameplayTask* UDGameplayAbility::FindTaskByName(FName TaskInstanceName)
{
	const int32 Index = ActiveTasks.IndexOfByPredicate(
		[TaskInstanceName](const TObjectPtr<UGameplayTask> Task) -> bool
		{
			return Task && Task->GetInstanceName() == TaskInstanceName;
		});

	return Index == INDEX_NONE ? nullptr : ActiveTasks[Index];
}
