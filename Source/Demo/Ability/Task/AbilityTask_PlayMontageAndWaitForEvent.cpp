// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityTask_PlayMontageAndWaitForEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Global/Statics.h"

void UAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendedIn(UAnimMontage* Montage) const
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnBlendedIn.Broadcast(FGameplayTag::EmptyTag, FGameplayEventData());
	}
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, const bool bInterrupted) const
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			ResetAnimRootMotionTranslationScale(1.f);
		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FGameplayTag::EmptyTag, FGameplayEventData());
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(FGameplayTag::EmptyTag, FGameplayEventData());
		}
	}
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnGameplayAbilityCancelled() const
{
	if (StopPlayingMontage() && ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast(FGameplayTag::EmptyTag, FGameplayEventData());
	}
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (!bInterrupted)
		{
			OnCompleted.Broadcast(FGameplayTag::EmptyTag, FGameplayEventData());
		}
		else
		{
			OnInterrupted.Broadcast(FGameplayTag::EmptyTag, FGameplayEventData());
		}
	}

	EndTask();
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) const
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

UAbilityTask_PlayMontageAndWaitForEvent* UAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
	UGameplayAbility* OwningAbility,
	UAnimMontage* MontageToPlay,
	const FGameplayTagContainer EventTags,
	const AActor* ExternalActor,
	const float Rate,
	const FName StartSection,
	const bool bStopWhenAbilityEnds,
	const float AnimRootMotionTranslationScale,
	const float StartTimeSeconds)
{
	ThisClass* Task = NewAbilityTask<ThisClass>(OwningAbility);
	Task->MontageToPlay = MontageToPlay;
	Task->EventTags = EventTags;
	Task->SetExternalTarget(ExternalActor);
	Task->Rate = Rate;
	Task->StartSection = StartSection;
	Task->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	Task->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	Task->StartTimeSeconds = StartTimeSeconds;
	return Task;
}

void UAbilityTask_PlayMontageAndWaitForEvent::ExternalCancel()
{
	OnGameplayAbilityCancelled();
	
	Super::ExternalCancel();
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnDestroy(const bool AbilityEnded)
{
	if (IsValid(Ability))
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	UAbilitySystemComponent* Asc = GetTargetAsc();
	if (IsValid(Asc))
	{
		Asc->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}
	
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_PlayMontageAndWaitForEvent::Activate()
{
	Super::Activate();
	
	if (!IsValid(Ability))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
		return;
	}

	UAbilitySystemComponent* Asc = GetTargetAsc();
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Asc"), __FUNCTION__));
		return;
	}

	if (!Asc->AbilityActorInfo.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid ActorInfo"), __FUNCTION__));
		return;
	}

	UAnimInstance* AnimInstance = Asc->AbilityActorInfo->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AnimInstance"), __FUNCTION__));
		return;
	}

	if (EventTags.IsValid())
	{
		EventHandle = Asc->AddGameplayEventTagContainerDelegate(EventTags,
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnGameplayEvent));
	}

	if (Asc->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
	{
		if (ShouldBroadcastAbilityTaskDelegates() == false)
		{
			return;
		}

		CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &ThisClass::OnGameplayAbilityCancelled);
		
		BlendedInDelegate.BindUObject(this, &ThisClass::OnMontageBlendedIn);
		AnimInstance->Montage_SetBlendedInDelegate(BlendedInDelegate, MontageToPlay);
		
		BlendingOutDelegate.BindUObject(this, &ThisClass::OnMontageBlendingOut);
		AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

		MontageEndedDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

		ResetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
	}
	else
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: called in Ability %s failed to play montage %s"),
			__FUNCTION__, *Ability->GetName(), *GetNameSafe(MontageToPlay)));
		
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

bool UAbilityTask_PlayMontageAndWaitForEvent::StopPlayingMontage() const
{
	const UAbilitySystemComponent* Asc = GetTargetAsc();
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Asc"), __FUNCTION__));
		return false;
	}
	
	const FGameplayAbilityActorInfo* ActorInfo = Asc->AbilityActorInfo.Get();
	if (!ActorInfo)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid ActorInfo"), __FUNCTION__));
		return false;
	}

	const UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AnimInstance"), __FUNCTION__));
		return false;
	}

	if (!IsValid(Ability))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
		return false;
	}
	
	if (AbilitySystemComponent->GetAnimatingAbility() == Ability && AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
	{
		if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
		{
			MontageInstance->OnMontageBlendingOutStarted.Unbind();
			MontageInstance->OnMontageEnded.Unbind();
		}

		AbilitySystemComponent->CurrentMontageStop();
		return true;
	}

	return false;
}

void UAbilityTask_PlayMontageAndWaitForEvent::SetExternalTarget(const AActor* Actor)
{
	if (IsValid(Actor))
	{
		bUseExternalTarget = true;
		OptionalExternalAsc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
}

UAbilitySystemComponent* UAbilityTask_PlayMontageAndWaitForEvent::GetTargetAsc() const
{
	return bUseExternalTarget ? OptionalExternalAsc.Get() : AbilitySystemComponent.Get();
}

void UAbilityTask_PlayMontageAndWaitForEvent::ResetAnimRootMotionTranslationScale(const float Scale) const
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	if (IsValid(Character) && (Character->GetLocalRole() == ROLE_Authority ||
		Character->GetLocalRole() == ROLE_AutonomousProxy &&
		Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted))
	{
		Character->SetAnimRootMotionTranslationScale(Scale);
	}
}
