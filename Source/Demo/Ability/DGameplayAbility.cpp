// Fill out your copyright notice in the Description page of Project Settings.

#include "DGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"

UDGameplayAbility::UDGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

FString UDGameplayAbility::GetModuleName_Implementation() const
{
	return TEXT("Ability.GameplayAbility");
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

	if (!Spec.GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityActivationAutoTag))
	{
		return;
	}

	if (Spec.Level <= 0)
	{
		UStatics::Log(this, ELogType::Log, FString::Printf(TEXT("%hs: AbilitySpec Level is less or equal than 0"), __FUNCTION__));
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
	const float Magnitude = LuaObtainCooldownMagnitude();
	EffectSpecHandle.Data->SetSetByCallerMagnitude(GlobalTags::SetByCallerCooldownValueTag, Magnitude);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
}

bool UDGameplayAbility::CheckCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	TMap<FGameplayTag, float> CostInfo;
	return !LuaObtainCostInfo(CostInfo) || CostInfo.IsEmpty() || Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

void UDGameplayAbility::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const UGameplayEffect* CostEffect = GetCostGameplayEffect();
	TMap<FGameplayTag, float> CostInfo;
	if (!IsValid(CostEffect) || !LuaObtainCostInfo(CostInfo) || CostInfo.IsEmpty())
	{
		return;
	}

	const FGameplayEffectSpecHandle& EffectSpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CostEffect->GetClass());
	if (EffectSpecHandle.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid EffectSpecHandle"), __FUNCTION__));
		return;
	}

	for (const auto& [CostTag, CostValue] : CostInfo)
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(CostTag, CostValue);
	}
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
}

UGameplayEffect* UDGameplayAbility::GetCooldownGameplayEffect() const
{
	UGameplayEffect* CooldownEffect = Super::GetCooldownGameplayEffect();
	if (IsValid(CooldownEffect))
	{
		return CooldownEffect;
	}
	
	return nullptr;
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

	const TWeakObjectPtr<UAbilitySystemComponent> Asc = ActorInfo->AbilitySystemComponent;
	if (!Asc.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	if (!Handle.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilitySpecHandle"), __FUNCTION__));
		return;
	}
	
	FGameplayAbilitySpec* AbilitySpec = Asc->FindAbilitySpecFromHandle(Handle);
	if (!AbilitySpec)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid FGameplayAbilitySpec"), __FUNCTION__));
		return;
	}

	Asc->RemoveLooseGameplayTags(AbilitySpec->GetDynamicSpecSourceTags());
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}