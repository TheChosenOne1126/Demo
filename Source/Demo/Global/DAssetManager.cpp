// Fill out your copyright notice in the Description page of Project Settings.

#include "DAssetManager.h"
#include "DemoSettings.h"
#include "Statics.h"
#include "DataAsset/PawnDataAsset.h"

UDAssetManager& UDAssetManager::Get()
{
	ThisClass* Singleton = Cast<ThisClass>(GEngine->AssetManager);

	checkf(::IsValid(Singleton), TEXT("Invalid DAssetManager"))
	
	return *Singleton;
}

UPawnDataAsset* UDAssetManager::GetPawnDataAsset() const
{
	return PawnDataAsset;
}

void UDAssetManager::LoadPawnData()
{
	const UDemoSettings* DemoSettings = GetDefault<UDemoSettings>();
	if (!::IsValid(DemoSettings))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Demo Settings"), __FUNCTION__));
		return;
	}

	if (DemoSettings->PawnDataAssetPath.IsNull())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Null Pawn Data Asset Path"), __FUNCTION__));
		return;
	}

	const FPrimaryAssetId PawnDataAssetId = GetPrimaryAssetIdForPath(DemoSettings->PawnDataAssetPath.ToSoftObjectPath());
	
	PawnDataHandle = LoadPrimaryAsset(PawnDataAssetId, TArray<FName>(), FStreamableDelegate::CreateWeakLambda(
		this, [this]() -> void
		{
			PawnDataAsset = PawnDataHandle.IsValid() ? PawnDataHandle->GetLoadedAsset<UPawnDataAsset>() : nullptr;
			OnPawnDataLoaded.Broadcast();
		}));
}

void UDAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
#if WITH_EDITOR
	if (::IsValid(PawnDataAsset))
	{
		return;
	}
	
	const UDemoSettings* DemoSettings = GetDefault<UDemoSettings>();
	if (!::IsValid(DemoSettings))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Demo Settings"), __FUNCTION__));
		return;
	}

	if (DemoSettings->PawnDataAssetPath.IsNull())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Null Pawn Data Asset Path"), __FUNCTION__));
		return;
	}
	
	PawnDataAsset = DemoSettings->PawnDataAssetPath.LoadSynchronous();
#endif
}
