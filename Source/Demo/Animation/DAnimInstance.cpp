// Fill out your copyright notice in the Description page of Project Settings.

#include "DAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "Global/StaticLib.h"

void UDAnimInstance::InitializeTagsByAbilitySystem(UAbilitySystemComponent* Asc)
{
	if (!IsValid(Asc))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid AbilitySystemComponent"), __FUNCTION__));
		return;
	}

	TagPropertyMap.Initialize(this, Asc);
}