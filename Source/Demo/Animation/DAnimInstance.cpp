// Fill out your copyright notice in the Description page of Project Settings.

#include "DAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "Global/Statics.h"

void UDAnimInstance::InitializeTagsByAbilitySystem(UAbilitySystemComponent* Asc)
{
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid AbilitySystemComponent"), __FUNCTION__));
		return;
	}

	TagPropertyMap.Initialize(this, Asc);
}

void UDAnimInstance::UpdateForwardAndSpeed()
{
	if (IsValid(GetWorld()) && GetWorld()->IsPreviewWorld())
	{
		return;
	}

	const APawn* Pawn = TryGetPawnOwner();
	if (!IsValid(Pawn))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid Pawn Owner!"), __FUNCTION__));
		return;
	}

	const FVector Velocity = Pawn->GetVelocity();

	const FVector Forward = Pawn->GetActorForwardVector();
	ForwardSpeed = Velocity.Dot(Forward) / Forward.SizeSquared();

	const FVector Right = Pawn->GetActorRightVector();
	RightSpeed = Velocity.Dot(Right) / Right.SizeSquared();
}
