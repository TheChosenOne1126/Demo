// Fill out your copyright notice in the Description page of Project Settings.

#include "DAbilityTask_PlayMontageWaitEvent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Global/Statics.h"

UDAbilityTask_PlayMontageWaitEvent* UDAbilityTask_PlayMontageWaitEvent::CreatePlayMontageAndWaitEvent(
	UGameplayAbility* OwningAbility,
	const FName TaskInstanceName,
	FGameplayTagContainer EventTags,
	UAnimMontage* MontageToPlay,
	const float Rate,
	const FName StartSection,
	const bool bStopWhenAbilityEnds,
	const float AnimRootMotionTranslationScale,
	const float StartTimeSeconds,
	const bool bAllowInterruptAfterBlendOut)
{
	ThisClass* AbilityTask = NewAbilityTask<ThisClass>(OwningAbility, TaskInstanceName);
	AbilityTask->EventTags = EventTags;
	AbilityTask->MontageToPlay = MontageToPlay;
	AbilityTask->Rate = Rate;
	AbilityTask->StartSection = StartSection;
	AbilityTask->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	AbilityTask->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	AbilityTask->bAllowInterruptAfterBlendOut = bAllowInterruptAfterBlendOut;
	AbilityTask->StartTimeSeconds = StartTimeSeconds;
	
	return AbilityTask;
}

void UDAbilityTask_PlayMontageWaitEvent::Activate()
{
	Super::Activate();

	if (!AbilitySystemComponent.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		EndTask();
		return;
	}

	if (!IsValid(Ability))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
		EndTask();
		return;
	}

	if (!IsValid(MontageToPlay))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MontageToPlay"), __FUNCTION__));
		EndTask();
		return;
	}
	
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AnimInstance"), __FUNCTION__));
		return;
	}

	ReceivedHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags,
		FGameplayEventTagMulticastDelegate::FDelegate::CreateWeakLambda(this,
			[this](const FGameplayTag EventTag, const FGameplayEventData* EventData) -> void
			{
				if (ShouldBroadcastAbilityTaskDelegates())
				{
					FGameplayEventData TempData = *EventData;
					TempData.EventTag = EventTag;

					OnReceived.Broadcast(EventTag, TempData);
				}
			}));
	
	if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(),
		MontageToPlay, Rate, StartSection, StartTimeSeconds) > 0.f)
	{
		if (!ShouldBroadcastAbilityTaskDelegates())
		{
			return;
		}

		InterruptedHandle = Ability->OnGameplayAbilityCancelled.AddWeakLambda(this,
			[this]() -> void
			{
				if (StopPlayingMontage() || bAllowInterruptAfterBlendOut)
				{
					if (ShouldBroadcastAbilityTaskDelegates())
					{
						bAllowInterruptAfterBlendOut = false;
						OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
					}
				}

				EndTask();
			});

		BlendedInDelegate.BindWeakLambda(this,
			[this](UAnimMontage*) -> void
			{
				if (ShouldBroadcastAbilityTaskDelegates())
				{
					OnBlendedIn.Broadcast(FGameplayTag(), FGameplayEventData());
				}
			});
		AnimInstance->Montage_SetBlendedInDelegate(BlendedInDelegate, MontageToPlay);

		BlendingOutDelegate.BindWeakLambda(this,
			[this](UAnimMontage* Montage, bool bInterrupted) -> void
			{
				const bool bPlayingThisMontage = Montage == MontageToPlay && Ability && Ability->GetCurrentMontage() == MontageToPlay;
				if (bPlayingThisMontage)
				{
					ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
					if (Character && (Character->GetLocalRole() == ROLE_Authority
						|| (Character->GetLocalRole() == ROLE_AutonomousProxy
							&& Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
					{
						Character->SetAnimRootMotionTranslationScale(1.f);
					}
				}

				if (bPlayingThisMontage && (bInterrupted || !bAllowInterruptAfterBlendOut))
				{
					if (AbilitySystemComponent.IsValid())
					{
						AbilitySystemComponent->ClearAnimatingAbility(Ability);
					}
				}

				if (ShouldBroadcastAbilityTaskDelegates())
				{
					if (bInterrupted)
					{
						bAllowInterruptAfterBlendOut = false;
						OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
						EndTask();
					}
					else
					{
						OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
					}
				}
			});
		AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

		MontageEndedDelegate.BindWeakLambda(this,
			[this](UAnimMontage*, bool bInterrupted) -> void
			{
				if (!bInterrupted)
				{
					if (ShouldBroadcastAbilityTaskDelegates())
					{
						OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
					}
				}
				else if(bAllowInterruptAfterBlendOut)
				{
					if (ShouldBroadcastAbilityTaskDelegates())
					{
						OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
					}
				}

				EndTask();
			});
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

		ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
		if (Character && (Character->GetLocalRole() == ROLE_Authority
			|| (Character->GetLocalRole() == ROLE_AutonomousProxy
				&& Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
		{
			Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
		}
	}
	else
	{
		UStatics::Log(this, ELogType::Error, FString::Printf( TEXT("%hs: Ability:%s failed to play montage:%s Task Instance Name:%s"),
			__FUNCTION__, *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString()));
		
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UDAbilityTask_PlayMontageWaitEvent::ExternalCancel()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}

	Super::ExternalCancel();
}

void UDAbilityTask_PlayMontageWaitEvent::OnDestroy(bool AbilityEnded)
{
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(InterruptedHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	if (AbilitySystemComponent.IsValid() && ReceivedHandle.IsValid())
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, ReceivedHandle);
	}

	Super::OnDestroy(AbilityEnded);
}

bool UDAbilityTask_PlayMontageWaitEvent::StopPlayingMontage() const
{
	if (!IsValid(Ability))
	{
		return false;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return false;
	}

	const UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		return false;
	}

	if (!AbilitySystemComponent.IsValid())
	{
		return false;
	}

	if (AbilitySystemComponent->GetAnimatingAbility() != Ability)
	{
		return false;
	}

	if (AbilitySystemComponent->GetCurrentMontage() != MontageToPlay)
	{
		return false;
	}

	if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
	{
		MontageInstance->OnMontageBlendedInEnded.Unbind();
		MontageInstance->OnMontageBlendingOutStarted.Unbind();
		MontageInstance->OnMontageEnded.Unbind();
	}

	AbilitySystemComponent->CurrentMontageStop();
	return true;
}