// Fill out your copyright notice in the Description page of Project Settings.

#include "DCharacter.h"
#include "DPlayerState.h"
#include "Component/DAbilitySystemComponent.h"
#include "Global/Statics.h"

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

void ADCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
	{
		return;
	}

	ADPlayerState* Ps = GetPlayerState<ADPlayerState>();
	if (!IsValid(Ps))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid DPlayerState"), __FUNCTION__));
		return;
	}

	UAbilitySystemComponent* Asc = GetAbilitySystemComponent();
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		return;
	}
	
	Asc->InitAbilityActorInfo(Ps, this);

	Ps->InitAbilitySystem(this);
}

void ADCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	UAbilitySystemComponent* Asc = GetAbilitySystemComponent();
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		return;
	}

	ADPlayerState* Ps = GetPlayerState<ADPlayerState>();
	if (!IsValid(Ps))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid DPlayerState"), __FUNCTION__));
		return;
	}
	
	Asc->InitAbilityActorInfo(Ps, this);

	if (!IsLocallyControlled())
	{
		Ps->RegisterAttributes();
	}
}