// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
	UGameDataAsset* GetGameDataAsset() const;
	
	FGameDataLoadedDelegate OnGameDataLoaded;

	void LoadGameData();
	
	virtual void StartInitialLoading() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UGameDataAsset> GameDataAsset;
	
	TSharedPtr<FStreamableHandle> PawnDataHandle;
};
