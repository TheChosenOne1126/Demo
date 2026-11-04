// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UnLuaInterface.h"
#include "Ability/DAbilityTypes.h"
#include "DAbilitySystemComponent.generated.h"

struct FAbilityData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegatePlayerControllerSet, APlayerController*, PlayerController);

UCLASS()
class DEMO_API UDAbilitySystemComponent : public UAbilitySystemComponent, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	virtual FString GetModuleName_Implementation() const override;

	virtual void OnPlayerControllerSet() override;

	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;

	void ConsumeReplicatedPayload(const FGameplayTag& Tag, const FGameplayAbilitySpecHandle& Handle, const FPredictionKey& Key) const;
	
	FGameplayTag GetReplicatedPayloadTag(const FGameplayTag& Tag, const FGameplayAbilitySpecHandle& Handle, const FPredictionKey& Key) const;
	
	void InvokeReplicatedPayload(
		const FGameplayTag& Tag,
		const FGameplayAbilitySpecHandle& Handle,
		const FPredictionKey& AbilityOriginalPredictionKey,
		const FPredictionKey& CurrentPredictionKey,
		const FGameplayTag& PayloadTag);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetReplicatedPayload(const FGameplayTag& Tag,
		const FGameplayAbilitySpecHandle& Handle,
		const FPredictionKey& AbilityOriginalPredictionKey,
		const FPredictionKey& CurrentPredictionKey,
		const FGameplayTag& PayloadTag);
	
	bool CallReplicatedEventDelegateIfSet(const FGameplayTag& Tag, const FGameplayAbilitySpecHandle& Handle, const FPredictionKey& Key) const;
	
	FSimpleMulticastDelegate& AbilityReplicatedPayloadDelegate(
		const FGameplayTag& Tag,
		const FGameplayAbilitySpecHandle& Handle,
		const FPredictionKey& Key);
	
	void ProcessAbilityInput();
	
	void AbilityForInputStart(FGameplayTag InputTag);
	
	void AbilityForInputTrigger(FGameplayTag InputTag);
	
	void AbilityForInputCancel(FGameplayTag InputTag);
	
	UFUNCTION()
	void GiveAbilityWithData(TSubclassOf<UGameplayAbility> AbilityClass, const FAbilityData& Data);
	
	UPROPERTY()
	FDelegatePlayerControllerSet PlayerControllerSet;
	
protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	
	virtual void InternalServerTryActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		bool InputPressed,
		const FPredictionKey& PredictionKey,
		const FGameplayEventData* TriggerEventData) override;
	
private:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> InputTriggerHandles;
	
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> InputStartHandles;
	
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> CachePressedHandles;
	
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> CacheHeldHandles;
	
	FDAbilityReplicatedPayloadContainer AbilityReplicatedPayloadContainer;
};

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
