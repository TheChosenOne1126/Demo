// Fill out your copyright notice in the Description page of Project Settings.

#include "GlobalUISubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Global/Statics.h"
#include "Interface/WidgetInterface.h"

void UGlobalUISubsystem::SetUpLoadingScreenWidget(TSubclassOf<UUserWidget> LoadingScreenWidgetClass)
{
	if (!IsValid(LoadingScreenWidgetClass))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid LoadingScreenWidgetClass"), __FUNCTION__));
		return;
	}

	if (!LoadingScreenWidgetClass->ImplementsInterface(UWidgetInterface::StaticClass()))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: LoadingScreenWidgetClass no implement UWidgetInterface"), __FUNCTION__));
		return;
	}
	
	const int32 ZOrder = static_cast<int32>(IWidgetInterface::Execute_GetZOrder(LoadingScreenWidgetClass));
	
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid LocalPlayer"), __FUNCTION__));
		return;
	}
	
	UGameInstance* GameInstance = LocalPlayer->GetGameInstance();
	if (!IsValid(GameInstance))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid GameInstance"), __FUNCTION__));
		return;
	}
	
	UUserWidget* Widget = UUserWidget::CreateWidgetInstance(*GameInstance, LoadingScreenWidgetClass, TEXT("LoadingScreenWidget"));
	if (!IsValid(Widget))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid LoadingScreenWidget"), __FUNCTION__));
		return;
	}
	
	UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient.Get();
	if (!IsValid(ViewportClient))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid ViewportClient"), __FUNCTION__));
		return;
	}

	ViewportClient->AddViewportWidgetForPlayer(LocalPlayer, Widget->TakeWidget(), ZOrder);
	LoadingScreenWidget = Widget->TakeWidget().ToSharedPtr();
}

void UGlobalUISubsystem::CloseLoadingScreenWidget()
{
	if (!LoadingScreenWidget.IsValid())
	{
		return;
	}
	
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid LocalPlayer"), __FUNCTION__));
		return;
	}
	
	UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient.Get();
	if (!IsValid(ViewportClient))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid ViewportClient"), __FUNCTION__));
		return;
	}

	if (!LoadingScreenWidget.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid LoadingScreenWidget"), __FUNCTION__));
		return;
	}
	
	ViewportClient->RemoveViewportWidgetForPlayer(LocalPlayer, LoadingScreenWidget.ToSharedRef());
	LoadingScreenWidget.Reset();
}
