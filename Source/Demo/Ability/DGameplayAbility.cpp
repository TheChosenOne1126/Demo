// Fill out your copyright notice in the Description page of Project Settings.

#include "DGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Global/GlobalTags.h"
#include "Global/StaticLib.h"

UDGameplayAbility::UDGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UDGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	if (IsValid(CooldownEffect) && CooldownTags.IsValid())
	{
		CooldownEffect->CachedGrantedTags.AppendTags(CooldownTags);
	}

	if (ActorInfo)
	{
		OnInitialized(*ActorInfo);
	}
	
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UDGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	if (IsValid(CooldownEffect) && CooldownTags.IsValid())
	{
		CooldownEffect->CachedGrantedTags.RemoveTags(CooldownTags);
	}
	
	if (ActorInfo)
	{
		OnDeInitialized(*ActorInfo);
	}
	
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UDGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (!ActorInfo->IsNetAuthority())
	{
		return;
	}

	if (!Spec.GetDynamicSpecSourceTags().HasTagExact(GlobalTags::GA_PASSIVE))
	{
		return;
	}

	if (Spec.Level <= 0)
	{
		UStaticLib::Log(this, ELogType::Log, FString::Printf(TEXT("%hs: AbilitySpec Level is less or equal than 0"), __FUNCTION__));
		return;
	}

	if (!ActorInfo)
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilityActorInfo"), __FUNCTION__));
		return;
	}
	
	if (!ActorInfo->AbilitySystemComponent.IsValid())
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UAbilitySystemComponent"), __FUNCTION__));
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
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid EffectSpecHandle"), __FUNCTION__));
		return;
	}
	
	EffectSpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
}

void UDGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo)
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilityActorInfo"), __FUNCTION__));
		return;
	}

	if (!ActorInfo->AbilitySystemComponent.IsValid())
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	if (!Handle.IsValid())
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilitySpecHandle"), __FUNCTION__));
		return;
	}
	
	FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (!AbilitySpec)
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilitySpec"), __FUNCTION__));
		return;
	}

	ActorInfo->AbilitySystemComponent->AddLooseGameplayTags(AbilitySpec->GetDynamicSpecSourceTags());

	for (const FGameplayTag& Tag : ActivationCancelTags)
	{
		ActivationCancelTagToHandleMap.Emplace(Tag) = ActorInfo->AbilitySystemComponent->RegisterGameplayTagEvent(Tag).AddUObject(
			this, &ThisClass::OnGameplayTagEvent);
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
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilityActorInfo"), __FUNCTION__));
		return;
	}

	const TWeakObjectPtr<UAbilitySystemComponent> Asc = ActorInfo->AbilitySystemComponent;
	if (!Asc.IsValid())
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	if (!Handle.IsValid())
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilitySpecHandle"), __FUNCTION__));
		return;
	}
	
	FGameplayAbilitySpec* AbilitySpec = Asc->FindAbilitySpecFromHandle(Handle);
	if (!AbilitySpec)
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilitySpec"), __FUNCTION__));
		return;
	}

	Asc->RemoveLooseGameplayTags(AbilitySpec->GetDynamicSpecSourceTags());
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDGameplayAbility::OnGameplayTagEvent(const FGameplayTag Tag, int32 Count)
{
	if (!ActivationCancelTagToHandleMap.Contains(Tag))
	{
		return;
	}
	
	if (Count > 0)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

FGameplayEffectSpecHandle UDGameplayAbility::MakeOutgoingGESpecByTag(const FGameplayTag& Tag, float Level) const
{
	if (!Tag.IsValid())
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Tag"), __FUNCTION__));
		return FGameplayEffectSpecHandle();
	}
	
	TSubclassOf<UGameplayEffect> GEClass;
	if (!UStaticLib::GetGEClassByTag(Tag, GEClass))
	{
		UStaticLib::Log(this, ELogType::Error,
			FString::Printf(TEXT("%hs: failed to get GEClass Tag: [%s]"), __FUNCTION__, *Tag.ToString()));
		return FGameplayEffectSpecHandle();
	}
	
	return MakeOutgoingGameplayEffectSpec(GEClass, Level);
}
