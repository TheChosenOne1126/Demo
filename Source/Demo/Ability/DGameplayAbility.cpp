// Fill out your copyright notice in the Description page of Project Settings.

#include "DGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Component/DAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"

UDGameplayAbility::UDGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UDGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

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

	const auto& Lambda = [this](const FGameplayTag Tag, int32 NewCount) -> void
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

	for (const FGameplayTag& ActivationCancelTag : ActivationCancelTags)
	{
		Asc->RegisterGameplayTagEvent(ActivationCancelTag).RemoveAll(this);
	}

	StopPlayMontage();

	Asc->RemoveGameplayEventTagContainerDelegate(EventTags, GameplayEventDelegateHandle);
	GameplayEventDelegateHandle.Reset();
	
	EventTags.Reset();
}

void UDGameplayAbility::LuaPlayMontage(
	UAnimMontage* Montage,
	float Rate,
	FName StartSection,
	float StartTimeSeconds)
{
	if (!IsValid(Montage))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Montage"), __FUNCTION__));
		return;
	}

	StopPlayMontage();
	
	UAbilitySystemComponent* Asc = GetAbilitySystemComponentFromActorInfo();
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	if (Asc->PlayMontage(this, CurrentActivationInfo, Montage, Rate, StartSection, StartTimeSeconds) <= 0.f)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: failed to play montage:%s"), __FUNCTION__, *GetNameSafe(Montage)));
		return;
	}

	if (!CurrentActorInfo)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid CurrentActorInfo"), __FUNCTION__));
		return;
	}
	
	UAnimInstance* AnimInstance = CurrentActorInfo->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AnimInstance"), __FUNCTION__));
		return;
	}

	FOnMontageBlendedInEnded BlendedInEndedDelegate;
	BlendedInEndedDelegate.BindWeakLambda(this,
		[this](UAnimMontage* BlendedInMontage) -> void
		{
			LuaOnMontageBlendedIn(BlendedInMontage);
		});
	AnimInstance->Montage_SetBlendedInDelegate(BlendedInEndedDelegate, Montage);

	FOnMontageBlendingOutStarted BlendingOutStartedDelegate;
	BlendingOutStartedDelegate.BindWeakLambda(this,
		[this, Asc, Montage](UAnimMontage* BlendingOutMontage, bool bInterrupted) -> void
		{
			if (BlendingOutMontage == Montage && GetCurrentMontage() == Montage)
			{
				ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
				if (Character && (Character->GetLocalRole() == ROLE_Authority || (Character->GetLocalRole() == ROLE_AutonomousProxy
					&& GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(1.f);
				}

				if (bInterrupted)
				{
					Asc->ClearAnimatingAbility(this);
				}
			}

			if (!bInterrupted)
			{
				LuaOnMontageBlendedOut(BlendingOutMontage);
			}
		});
	AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutStartedDelegate, Montage);

	FOnMontageEnded EndedDelegate;
	EndedDelegate.BindWeakLambda(this,
		[this](UAnimMontage* EndedMontage, bool bInterrupted) -> void
		{
			if (!bInterrupted)
			{
				LuaOnMontageEnd(EndedMontage);
			}
		});
	AnimInstance->Montage_SetEndDelegate(EndedDelegate, Montage);

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (Character && (Character->GetLocalRole() == ROLE_Authority || (Character->GetLocalRole() == ROLE_AutonomousProxy
		&& GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
	{
		Character->SetAnimRootMotionTranslationScale(1.f);
	}
}

void UDGameplayAbility::LuaWaitGameplayEvent(const FGameplayTagContainer& InEventTags, bool bSyncToServer)
{
	UDAbilitySystemComponent* Asc = Cast<UDAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	GameplayEventDelegateHandle = Asc->AddGameplayEventTagContainerDelegate(InEventTags, 
		FGameplayEventTagMulticastDelegate::FDelegate::CreateWeakLambda(this,
			[this, Asc, bSyncToServer](FGameplayTag EventTag, const FGameplayEventData* EventData) -> void
			{
				if (!EventData)
				{
					return;
				}
				
				LuaOnEventHandle(EventTag, *EventData);

				if (bSyncToServer && IsPredictingClient())
				{
					Asc->ServerHandleGameplayEvent(EventTag, *EventData);
				}
			}));

	EventTags.AppendTags(InEventTags);
}

void UDGameplayAbility::SetupActivationCancelTags(const FGameplayTagContainer& InActivationCancelTags)
{
	if (!InActivationCancelTags.IsValid())
	{
		return;
	}
	
	ActivationCancelTags = InActivationCancelTags;
	
	ActivationBlockedTags.AppendTags(InActivationCancelTags);
}

void UDGameplayAbility::StopPlayMontage()
{
	if (!IsValid(CurrentMontage))
	{
		return;
	}
	
	UAbilitySystemComponent* Asc = GetAbilitySystemComponentFromActorInfo();
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	if (Asc->GetAnimatingAbility() != this)
	{
		UStatics::Log(this, ELogType::Warning, FString::Printf(TEXT("%hs: animating ability is not {%s}, is {%s}"),
			__FUNCTION__, *GetNameSafe(this), *GetNameSafe(Asc->GetAnimatingAbility())));
		return;
	}

	if (!CurrentActorInfo)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid CurrentActorInfo"), __FUNCTION__));
		return;
	}

	const UAnimInstance* AnimInstance = CurrentActorInfo->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AnimInstance"), __FUNCTION__));
		return;
	}

	if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(CurrentMontage))
	{
		MontageInstance->OnMontageBlendedInEnded.Unbind();
		MontageInstance->OnMontageBlendingOutStarted.Unbind();
		MontageInstance->OnMontageEnded.Unbind();
	}

	MontageStop();
}
