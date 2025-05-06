// Fill out your copyright notice in the Description page of Project Settings.

#include "DPlayerController.h"
#include "DPlayerState.h"
#include "Component/DAbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Global/Statics.h"
#include "UI/HUDWidget.h"

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

void ADPlayerController::CreateHUD(const TArray<FAbilitySlotData>& AbilitySlotDataArr)
{
	if (IsValid(HUDWidget))
	{
		return;
	}

	if (HUDWidgetClass.IsNull())
	{
		return;
	}

	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	StreamableManager.RequestAsyncLoad(HUDWidgetClass.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
		[this, AbilitySlotDataArr]() -> void
		{
			UClass* LoadedClass = HUDWidgetClass.Get();
			if (IsValid(LoadedClass))
			{
				HUDWidget = CreateWidget<UHUDWidget>(this, LoadedClass, TEXT("HUDWidget"));
				HUDWidget->InitAbilitySlot(AbilitySlotDataArr);
			}
		}));
}
