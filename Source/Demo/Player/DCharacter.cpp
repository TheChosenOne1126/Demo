// Fill out your copyright notice in the Description page of Project Settings.

#include "DCharacter.h"
#include "Component/DAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

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
