// Fill out your copyright notice in the Description page of Project Settings.

#include "DPlayerController.h"
#include "DPlayerState.h"
#include "Component/DAbilitySystemComponent.h"
#include "Global/Statics.h"

void ADPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);

	const ADPlayerState* Ps = GetPlayerState<ADPlayerState>();
	if (!IsValid(Ps))
	{
		return;
	}

	UDAbilitySystemComponent* Asc = Cast<UDAbilitySystemComponent>(Ps->GetAbilitySystemComponent());
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	Asc->ProcessAbilityInput();
}
