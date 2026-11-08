// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UnLuaInterface.h"
#include "Ability/DAbilityTypes.h"
#include "DAbilitySystemComponent.generated.h"

UCLASS()
class DEMO_API UDAbilitySystemComponent : public UAbilitySystemComponent, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	virtual FString GetModuleName_Implementation() const override;

	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;

	void ConsumeReplicatedPayload(const FGameplayTag& Tag, const FGameplayAbilitySpecHandle& Handle, const FPredictionKey& Key) const;
	
	FGameplayTag GetReplicatedPayloadTag(const FGameplayTag& Tag, const FGameplayAbilitySpecHandle& Handle, const FPredictionKey& Key) const;
	
	void InvokeReplicatedPayload(
		const FGameplayTag& Tag,
		const FGameplayTag& PayloadTag,
		const FGameplayAbilitySpecHandle& Handle,
		const FPredictionKey& AbilityOriginalPredictionKey,
		const FPredictionKey& CurrentPredictionKey = FPredictionKey());
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetReplicatedPayload(const FGameplayTag& Tag,
		const FGameplayTag& PayloadTag,
		const FGameplayAbilitySpecHandle& Handle,
		const FPredictionKey& AbilityOriginalPredictionKey,
		const FPredictionKey& CurrentPredictionKey);
	
	bool CallReplicatedEventDelegateIfSet(const FGameplayTag& Tag, const FGameplayAbilitySpecHandle& Handle, const FPredictionKey& Key) const;
	
	FSimpleMulticastDelegate& AbilityReplicatedPayloadDelegate(
		const FGameplayTag& Tag,
		const FGameplayAbilitySpecHandle& Handle,
		const FPredictionKey& Key);
	
	void ProcessAbilityInput();
	
	void AbilityForInputStart(const FGameplayTag& InputTag);
	
	void AbilityForInputTrigger(const FGameplayTag& InputTag);
	
	void AbilityForInputCancel(const FGameplayTag& InputTag);

protected:
	virtual void InternalServerTryActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		bool InputPressed,
		const FPredictionKey& PredictionKey,
		const FGameplayEventData* TriggerEventData) override;
	
private:
	UPROPERTY()
	TMap<FGameplayAbilitySpecHandle, FGameplayTag> InputStartHandleToTagMap;
	
	UPROPERTY()
	TMap<FGameplayAbilitySpecHandle, FGameplayTag> InputTriggerHandleToTagMap;
	
	UPROPERTY()
	TMap<FGameplayAbilitySpecHandle, FGameplayTag> InputCancelHandleToTagMap;
	
	FDAbilityReplicatedPayloadContainer AbilityReplicatedPayloadContainer;
};

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
