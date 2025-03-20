// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "DAbilityTask_PlayMontageWaitEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FPlayMontageWaitEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

UCLASS()
class DEMO_API UDAbilityTask_PlayMontageWaitEvent : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DAbility|Task", meta = (DisplayName = "PlayMontageWaitEvent",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = true))
	static UDAbilityTask_PlayMontageWaitEvent* CreatePlayMontageAndWaitEvent(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		FGameplayTagContainer EventTags,
		UAnimMontage* MontageToPlay,
		float Rate = 1.f,
		FName StartSection = NAME_None,
		bool bStopWhenAbilityEnds = true,
		float AnimRootMotionTranslationScale = 1.f,
		float StartTimeSeconds = 0.f,
		bool bAllowInterruptAfterBlendOut = false);

	virtual void OnDestroy(bool AbilityEnded) override;

	virtual void ExternalCancel() override;

protected:
	virtual void Activate() override;

	bool StopPlayingMontage() const;

	UPROPERTY(BlueprintAssignable)
	FPlayMontageWaitEventDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FPlayMontageWaitEventDelegate OnBlendedIn;

	UPROPERTY(BlueprintAssignable)
	FPlayMontageWaitEventDelegate OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FPlayMontageWaitEventDelegate OnCancelled;

	UPROPERTY(BlueprintAssignable)
	FPlayMontageWaitEventDelegate OnReceived;

	UPROPERTY(BlueprintAssignable)
	FPlayMontageWaitEventDelegate OnInterrupted;

private:
	FOnMontageBlendedInEnded BlendedInDelegate;

	FOnMontageBlendingOutStarted BlendingOutDelegate;

	FOnMontageEnded MontageEndedDelegate;

	FDelegateHandle InterruptedHandle;

	FDelegateHandle ReceivedHandle;

	UPROPERTY()
	TObjectPtr<UAnimMontage> MontageToPlay;

	UPROPERTY()
	float Rate;

	UPROPERTY()
	FName StartSection;

	UPROPERTY()
	float AnimRootMotionTranslationScale;

	UPROPERTY()
	float StartTimeSeconds;

	UPROPERTY()
	bool bStopWhenAbilityEnds;

	UPROPERTY()
	bool bAllowInterruptAfterBlendOut;

	UPROPERTY()
	FGameplayTagContainer EventTags;
};
