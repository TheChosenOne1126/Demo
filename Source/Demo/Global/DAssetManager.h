// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "DAssetManager.generated.h"

class UPawnDataAsset;

UCLASS(Config = Game)
class DEMO_API UDAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UDAssetManager& Get();
	
	UPawnDataAsset* GetPawnDataAsset() const;
	
	FSimpleMulticastDelegate OnPawnDataLoaded;

	void LoadPawnData();

private:
	UPROPERTY(Transient)
	TObjectPtr<UPawnDataAsset> PawnDataAsset;
	
	TSharedPtr<FStreamableHandle> PawnDataHandle;
};
