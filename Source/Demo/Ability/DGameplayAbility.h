// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DGameplayAbility.generated.h"

UCLASS(Abstract)
class DEMO_API UDGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDGameplayAbility();
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual bool CheckCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual bool CheckCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ApplyCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
	
	UFUNCTION()
	void LuaPlayMontage(
		UAnimMontage* Montage,
		float Rate,
		FName StartSection,
		float StartTimeSeconds);
	
	UFUNCTION()
	void LuaWaitGameplayEvent(const FGameplayTagContainer& InEventTags, bool bSyncToServer);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	void LuaOnMontageBlendedIn(UAnimMontage* Montage);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	void LuaOnMontageBlendedOut(UAnimMontage* Montage);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	void LuaOnMontageEnd(UAnimMontage* Montage);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	float LuaObtainCooldownMagnitude() const;
	
	UPROPERTY()
	FGameplayTagContainer CooldownTags;
	
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	void LuaOnEventHandle(FGameplayTag EventTag, const FGameplayEventData& EventData);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	bool LuaObtainCostInfo(TMap<FGameplayTag, float>& OutCostInfo) const;
	
	UFUNCTION()
	void SetupActivationCancelTags(const FGameplayTagContainer& InActivationCancelTags);

private:
	void StopPlayMontage();
	
	FDelegateHandle ActivationCancelTagsDelegateHandle;
	
	UPROPERTY(Transient)
	FGameplayTagContainer EventTags;
	
	FDelegateHandle GameplayEventDelegateHandle;
	
	UPROPERTY()
	FGameplayTagContainer ActivationCancelTags;
};
