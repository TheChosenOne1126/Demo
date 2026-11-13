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

	const FPrimaryAssetId PawnDataAssetId = GetPrimaryAssetIdForPath(DemoSettings->GameDataAssetPath.ToSoftObjectPath());
	
	PawnDataHandle = LoadPrimaryAsset(PawnDataAssetId, TArray<FName>(), FStreamableDelegate::CreateWeakLambda(
		this, [this]() -> void
		{
			GameDataAsset = PawnDataHandle.IsValid() ? PawnDataHandle->GetLoadedAsset<UGameDataAsset>() : nullptr;
			OnGameDataLoaded.Broadcast(GameDataAsset);
			
			for (const FGameplayEffectType& GEType : GameDataAsset->GameplayEffectTypes)
			{
				GEClassMap.Emplace(GEType.Tag, GEType.GameplayEffectClass);
			}
			
			if (!::IsValid(GameDataAsset->UIDataAsset))
			{
				UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UIDataAsset"), __FUNCTION__));
				return;
			}

			for (const FUIType& UIType : GameDataAsset->UIDataAsset->UITypes)
			{
				UIDataConfigMap.Emplace(UIType.UITag, UIType.Config);
			}
			
			for (const FCameraModifierType& CMType : GameDataAsset->CameraModifierTypes)
			{
				CMClassMap.Emplace(CMType.Tag, CMType.CameraModifierClass);
			}
		}));
}

bool UDAssetManager::TryGetUIDataConfigByTag(const FGameplayTag& Tag, TSubclassOf<UUserWidget>& Class, int& ZOrder) const
{
	const FUIDataConfig* Property = UIDataConfigMap.Find(Tag);
	if (!Property)
	{
		return false;
	}
	
	Class = Property->WidgetClass;
	ZOrder = Property->ZOrder;
	return true;
}

bool UDAssetManager::TryGetGEClassByTag(const FGameplayTag& Tag, TSubclassOf<UGameplayEffect>& Class) const
{
	if (GEClassMap.Contains(Tag))
	{
		Class = GEClassMap.FindRef(Tag);
		return true;
	}
	
	return false;
}

bool UDAssetManager::TryGetCMClassByTag(const FGameplayTag& Tag, TSubclassOf<UCameraModifier>& Class) const
{
	if (CMClassMap.Contains(Tag))
	{
		Class = CMClassMap.FindRef(Tag);
		return true;
	}
	
	return false;
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
