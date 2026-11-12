// Fill out your copyright notice in the Description page of Project Settings.

#include "GlobalUISubsystem.h"
#include "Blueprint/UserWidget.h"
#include "DataAsset/UIDataAsset.h"
#include "Global/DAssetManager.h"
#include "Global/Statics.h"
#include "Interface/WidgetInterface.h"

void UGlobalUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UGameDataAsset* DataAsset = UDAssetManager::Get().GetGameDataAsset();
	if (!IsValid(DataAsset))
	{
		OnGameDataLoadedHandle = UDAssetManager::Get().OnGameDataLoaded.AddUObject(this, &UGlobalUISubsystem::OnGameDataLoadedCallback);
	}
	else
	{
		OnGameDataLoadedCallback(DataAsset);
	}
}

void UGlobalUISubsystem::Deinitialize()
{
	if (OnGameDataLoadedHandle.IsValid())
	{
		UDAssetManager::Get().OnGameDataLoaded.Remove(OnGameDataLoadedHandle);
		OnGameDataLoadedHandle.Reset();
	}
	
	Super::Deinitialize();
}

UUserWidget* UGlobalUISubsystem::CreateUIWidget(FGameplayTag UITag)
{
	if (!UITag.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UITag"), __FUNCTION__));
		return nullptr;
	}

	if (TagToInstanceMap.Contains(UITag))
	{
		return TagToInstanceMap.FindRef(UITag).Get();
	}

	const TSubclassOf<UUserWidget> WidgetClass = TagToClassMap.FindChecked(UITag);
	if (!IsValid(WidgetClass))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid GameInstance"), __FUNCTION__));
		return nullptr;
	}
	
	int ZOrder = TagToZOrderMap.FindRef(UITag);
	
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid LocalPlayer"), __FUNCTION__));
		return nullptr;
	}
	
	UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient.Get();
	if (!IsValid(ViewportClient))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid ViewportClient"), __FUNCTION__));
		return nullptr;
	}
	
	APlayerController* PC = LocalPlayer->GetPlayerController(GetWorld());
	if (!IsValid(PC))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid PlayerController"), __FUNCTION__));
		return nullptr;
	}
	
	UUserWidget* Widget = UUserWidget::CreateWidgetInstance(*PC, WidgetClass, TEXT("LoadingScreenWidget"));
	if (!IsValid(Widget))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid LoadingScreenWidget"), __FUNCTION__));
		return nullptr;
	}
	
	ViewportClient->AddViewportWidgetForPlayer(LocalPlayer, Widget->TakeWidget(), ZOrder);
	TagToInstanceMap.Emplace(UITag, Widget);
	
	return Widget;
}

void UGlobalUISubsystem::CloseUIWidget(FGameplayTag UITag)
{
	if (!UITag.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UITag"), __FUNCTION__));
		return;
	}

	if (!TagToInstanceMap.Contains(UITag))
	{
		UStatics::Log(this, ELogType::Error,
			FString::Printf(TEXT("%hs: TagToInstanceMap no UITag: [%s]"), __FUNCTION__, *UITag.ToString()));
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

	const TWeakObjectPtr<UUserWidget> Widget = TagToInstanceMap.FindRef(UITag);
	if (!Widget.IsValid())
	{
		return;
	}
	
	ViewportClient->RemoveViewportWidgetForPlayer(LocalPlayer, Widget->TakeWidget());
}

void UGlobalUISubsystem::OnGameDataLoadedCallback(const UGameDataAsset* GameDataAsset)
{
	if (!IsValid(GameDataAsset))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid GameDataAsset"), __FUNCTION__));
		return;
	}
	
	if (!IsValid(GameDataAsset->UIDataAsset))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UIDataAsset"), __FUNCTION__));
		return;
	}

	for (const FUIType& UIType : GameDataAsset->UIDataAsset->UITypes)
	{
		TagToClassMap.Emplace(UIType.UITag, UIType.WidgetClass);
		TagToZOrderMap.Emplace(UIType.UITag, UIType.ZOrder);
	}
}
