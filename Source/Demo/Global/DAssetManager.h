// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "DataAsset/UIDataAsset.h"
#include "Engine/AssetManager.h"
#include "DAssetManager.generated.h"

class UGameDataAsset;

DECLARE_MULTICAST_DELEGATE_OneParam(FGameDataLoadedDelegate, const UGameDataAsset*);

UCLASS(Config = Game)
class DEMO_API UDAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UDAssetManager& Get();
	
	virtual void StartInitialLoading() override;
	
	UGameDataAsset* GetGameDataAsset() const;
	
	FGameDataLoadedDelegate OnGameDataLoaded;

	void LoadGameData();
	
	bool TryGetUIDataConfigByTag(const FGameplayTag& Tag, OUT TSubclassOf<UUserWidget>& Class, OUT int& ZOrder) const;
	
	bool TryGetGEClassByTag(const FGameplayTag& Tag, OUT TSubclassOf<UGameplayEffect>& Class) const;
	
	bool TryGetCMClassByTag(const FGameplayTag& Tag, OUT TSubclassOf<UCameraModifier>& Class) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UGameDataAsset> GameDataAsset;
	
	TSharedPtr<FStreamableHandle> PawnDataHandle;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, FUIDataConfig> UIDataConfigMap;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> GEClassMap;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, TSubclassOf<UCameraModifier>> CMClassMap;
};
