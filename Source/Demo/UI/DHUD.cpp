// Fill out your copyright notice in the Description page of Project Settings.

#include "DHUD.h"
#include "HUDWidget.h"
#include "Player/DPlayerState.h"

DEFINE_LOG_CATEGORY(LogDHUD)

void ADHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(PlayerOwner))
	{
		UE_LOG(LogDHUD, Error, TEXT("%hs: invalid Player Controller"), __FUNCTION__)
		return;
	}

	HUDWidget = CreateWidget<UHUDWidget>(PlayerOwner, HUDWidgetClass, TEXT("HUDWidget"));
	if (!IsValid(HUDWidget))
	{
		UE_LOG(LogDHUD, Error, TEXT("%hs: invalid HUDWidget"), __FUNCTION__)
		return;
	}
	
	HUDWidget->AddToViewport();
}
