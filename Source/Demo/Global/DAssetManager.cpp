// Fill out your copyright notice in the Description page of Project Settings.

#include "DAssetManager.h"
#include "DemoSettings.h"
#include "Statics.h"
#include "DataAsset/PawnDataAsset.h"

UDAssetManager& UDAssetManager::GetRef()
{
	ThisClass* Singleton = Cast<ThisClass>(GEngine->AssetManager);

	checkf(::IsValid(Singleton), TEXT("Invalid DAssetManager"))
	
	return *Singleton;
}

void UDAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	LoadPawnData();
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

#if WITH_EDITOR
	FScopedSlowTask SlowTask(0.f, FText::FromString("Loading Pawn Data Asset"));
	SlowTask.MakeDialog(false, true);
	PawnDataAsset = DemoSettings->PawnDataAssetPath.LoadSynchronous();
	LoadPrimaryAssetsWithType(GetFNameSafe(UPawnDataAsset::StaticClass()));
#else
	const TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(GetFNameSafe(UPawnDataAsset::StaticClass()));
	if (Handle.IsValid())
	{
		Handle->WaitUntilComplete(0.f, false);
		PawnDataAsset = Handle->GetLoadedAsset<UPawnDataAsset>();
	}
#endif
}
