// Fill out your copyright notice in the Description page of Project Settings.

#include "WidgetLPSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "DataAsset/UIDataAsset.h"
#include "Global/DAssetManager.h"
#include "Global/StaticLib.h"

UUserWidget* UWidgetLPSubsystem::CreateUIWidget(FGameplayTag UITag)
{
	if (!UITag.IsValid())
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UITag"), __FUNCTION__));
		return nullptr;
	}

	if (TagToInstanceMap.Contains(UITag))
	{
		return TagToInstanceMap.FindRef(UITag).Get();
	}
	
	TSubclassOf<UUserWidget> WidgetClass;
	int ZOrder = 0;
	if (!UDAssetManager::Get().TryGetUIDataConfigByTag(UITag, WidgetClass, ZOrder))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(
			TEXT("%hs: failed to get UIDataConfig Tag: [%s]"), __FUNCTION__, *UITag.ToString()));
		return nullptr;
	}

	if (!IsValid(WidgetClass))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid GameInstance"), __FUNCTION__));
		return nullptr;
	}

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid LocalPlayer"), __FUNCTION__));
		return nullptr;
	}
	
	UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient.Get();
	if (!IsValid(ViewportClient))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid ViewportClient"), __FUNCTION__));
		return nullptr;
	}
	
	APlayerController* PC = LocalPlayer->GetPlayerController(GetWorld());
	if (!IsValid(PC))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid PlayerController"), __FUNCTION__));
		return nullptr;
	}
	
	UUserWidget* Widget = UUserWidget::CreateWidgetInstance(*PC, WidgetClass, TEXT("LoadingScreenWidget"));
	if (!IsValid(Widget))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid LoadingScreenWidget"), __FUNCTION__));
		return nullptr;
	}
	
	ViewportClient->AddViewportWidgetForPlayer(LocalPlayer, Widget->TakeWidget(), ZOrder);
	TagToInstanceMap.Emplace(UITag, Widget);
	
	return Widget;
}

void UWidgetLPSubsystem::CloseUIWidget(FGameplayTag UITag)
{
	if (!UITag.IsValid())
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UITag"), __FUNCTION__));
		return;
	}

	if (!TagToInstanceMap.Contains(UITag))
	{
		UStaticLib::Log(this, ELogType::Error,
			FString::Printf(TEXT("%hs: TagToInstanceMap no UITag: [%s]"), __FUNCTION__, *UITag.ToString()));
		return;
	}
	
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid LocalPlayer"), __FUNCTION__));
		return;
	}
	
	UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient.Get();
	if (!IsValid(ViewportClient))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid ViewportClient"), __FUNCTION__));
		return;
	}

	const TWeakObjectPtr<UUserWidget> Widget = TagToInstanceMap.FindRef(UITag);
	if (!Widget.IsValid())
	{
		return;
	}
	
	ViewportClient->RemoveViewportWidgetForPlayer(LocalPlayer, Widget->TakeWidget());
}