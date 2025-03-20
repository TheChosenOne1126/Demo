// Fill out your copyright notice in the Description page of Project Settings.

#include "DCommonUserWidget.h"
#include "Global/Statics.h"
#include "Player/DPlayerState.h"

UBaseAttributeViewModel* UDCommonUserWidget::GetBaseAttributeViewModel() const
{
	const ADPlayerState* PlayerState = GetOwningPlayerState<ADPlayerState>();
	if (!IsValid(PlayerState))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid DPlayerState"), __FUNCTION__));
		return nullptr;
	}

	return PlayerState->BaseAttributeViewModel;
}
