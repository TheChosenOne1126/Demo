// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_PlayMontageAndWaitForEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayMontageAndWaitForEventDelegate, const FGameplayTag&, EventTag, const FGameplayEventData&, Payload);

UCLASS()
class DEMO_API UAbilityTask_PlayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FPlayMontageAndWaitForEventDelegate	OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FPlayMontageAndWaitForEventDelegate	OnBlendedIn;

	UPROPERTY(BlueprintAssignable)
	FPlayMontageAndWaitForEventDelegate	OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FPlayMontageAndWaitForEventDelegate	OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FPlayMontageAndWaitForEventDelegate	OnCancelled;
	
	UPROPERTY(BlueprintAssignable)
	FPlayMontageAndWaitForEventDelegate EventReceived;

	void OnMontageBlendedIn(UAnimMontage* Montage);

	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	void OnGameplayAbilityCancelled();

	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);
	
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(
		UGameplayAbility* OwningAbility,
		UAnimMontage* MontageToPlay,
		FGameplayTagContainer EventTags,
		const AActor* ExternalActor = nullptr,
		float Rate = 1.f,
		FName StartSection = NAME_None,
		bool bStopWhenAbilityEnds = true,
		float AnimRootMotionTranslationScale = 1.f,
		float StartTimeSeconds = 0.f);

	virtual void ExternalCancel() override;
	
	virtual void OnDestroy(bool AbilityEnded) override;

protected:
	virtual void Activate() override;

	bool StopPlayingMontage();
	
	void SetExternalTarget(const AActor* Actor);
	
	UAbilitySystemComponent* GetTargetAsc();
	
	void ResetAnimRootMotionTranslationScale(float Scale);

	FOnMontageBlendedInEnded BlendedInDelegate;
	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;

	UPROPERTY()
	TObjectPtr<UAnimMontage> MontageToPlay;
	
	UPROPERTY()
	FGameplayTagContainer EventTags;
	
	UPROPERTY()
	float Rate = 1.f;
	
	UPROPERTY()
	FName StartSection;
	
	UPROPERTY()
	float AnimRootMotionTranslationScale;
	
	UPROPERTY()
	float StartTimeSeconds;
	
	UPROPERTY()
	uint8 bStopWhenAbilityEnds : 1;
	
	UPROPERTY()
	uint8 bUseExternalTarget : 1;
	
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> OptionalExternalAsc;
};
