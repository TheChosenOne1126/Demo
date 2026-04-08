// Fill out your copyright notice in the Description page of Project Settings.

#include "DAssetManager.h"
#include "DemoSettings.h"
#include "Statics.h"
#include "DataAsset/GameDataAsset.h"

UDAssetManager& UDAssetManager::Get()
{
	ThisClass* Singleton = Cast<ThisClass>(GEngine->AssetManager);

	checkf(::IsValid(Singleton), TEXT("Invalid DAssetManager"))
	
	return *Singleton;
}

UGameDataAsset* UDAssetManager::GetGameDataAsset() const
{
	return GameDataAsset;
}

void UDAssetManager::LoadGameData()
{
	const UDemoSettings* DemoSettings = GetDefault<UDemoSettings>();
	if (!::IsValid(DemoSettings))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Demo Settings"), __FUNCTION__));
		return;
	}

	if (DemoSettings->GameDataAssetPath.IsNull())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Null Pawn Data Asset Path"), __FUNCTION__));
		return;
	}

	const FPrimaryAssetId PawnDataAssetId = GetPrimaryAssetIdForPath(DemoSettings->GameDataAssetPath.ToSoftObjectPath());
	
	PawnDataHandle = LoadPrimaryAsset(PawnDataAssetId, TArray<FName>(), FStreamableDelegate::CreateWeakLambda(
		this, [this]() -> void
		{
			GameDataAsset = PawnDataHandle.IsValid() ? PawnDataHandle->GetLoadedAsset<UGameDataAsset>() : nullptr;
			OnGameDataLoaded.Broadcast();
		}));
}

void UDAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
#if WITH_EDITOR
	if (::IsValid(GameDataAsset))
	{
		return;
	}
	
	const UDemoSettings* DemoSettings = GetDefault<UDemoSettings>();
	if (!::IsValid(DemoSettings))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Demo Settings"), __FUNCTION__));
		return;
	}

	if (DemoSettings->GameDataAssetPath.IsNull())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Null Pawn Data Asset Path"), __FUNCTION__));
		return;
	}
	
	GameDataAsset = DemoSettings->GameDataAssetPath.LoadSynchronous();
#endif
}
