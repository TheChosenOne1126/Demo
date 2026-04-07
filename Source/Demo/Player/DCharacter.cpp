// Fill out your copyright notice in the Description page of Project Settings.

#include "DCharacter.h"
#include "DPlayerState.h"
#include "Global/Statics.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

UAbilitySystemComponent* ADCharacter::GetAbilitySystemComponent() const
{
	if (!IsValid(GetPlayerState()))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid PlayerState"), __FUNCTION__));
		return nullptr;
	}
	
	const IAbilitySystemInterface* Asi = GetPlayerState<IAbilitySystemInterface>();
	if (!Asi)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: PlayerState is not inherit AbilitySystemInterface"), __FUNCTION__));
		return nullptr;
	}
	
	return Asi->GetAbilitySystemComponent();
}

void ADCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnTag, Params);
}

void ADCharacter::SetPawnTag(const FGameplayTag& NewPawnTag)
{
	PawnTag = NewPawnTag;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnTag, this);
}

const FGameplayTag& ADCharacter::GetPawnTag() const
{
	return PawnTag;
}
