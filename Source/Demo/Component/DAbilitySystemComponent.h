// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UnLuaInterface.h"
#include "DAbilitySystemComponent.generated.h"

struct FAbilityData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegatePlayerControllerSet, APlayerController*, PlayerController);

UCLASS()
class DEMO_API UDAbilitySystemComponent : public UAbilitySystemComponent, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	virtual void DestroyActiveState() override;
	
	virtual FString GetModuleName_Implementation() const override;
	
	virtual void OnPlayerControllerSet() override;
	
	UFUNCTION()
	void GiveAbilityWithData(TSubclassOf<UGameplayAbility> AbilityClass, const FAbilityData& Data);
	
	UPROPERTY()
	FDelegatePlayerControllerSet PlayerControllerSet;
};

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
