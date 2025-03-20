// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DAbilitySystemComponent.generated.h"

UCLASS()
class DEMO_API UDAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	void AbilityForInputPressed(const FGameplayTag& InputTag);

	void AbilityForInputReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput();

	template<typename TOwner, typename... VarTypes>
	void RegisterAttributeValueChange(
		const FGameplayAttribute& Attribute,
		TOwner* Owner,
		void(TOwner::* Callback)(const FOnAttributeChangeData&, VarTypes...),
		VarTypes&&... Vars);

	void UnregisterAllAttributeValuesChange();

private:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> InputPressedHandles;

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> InputHeldHandles;

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> InputReleasedHandles;

	TMap<FGameplayAttribute, FDelegateHandle> AttributeValueChangeDelegateMap;
};

template <typename TOwner, typename ... VarTypes>
void UDAbilitySystemComponent::RegisterAttributeValueChange(
	const FGameplayAttribute& Attribute,
	TOwner* Owner,
	void(TOwner::* Callback)(const FOnAttributeChangeData&, VarTypes...),
	VarTypes&&... Vars)
{
	AttributeValueChangeDelegateMap.Emplace(Attribute) = GetGameplayAttributeValueChangeDelegate(Attribute)
		.AddUObject(Owner, Callback, Forward<VarTypes>(Vars)...);
}

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
