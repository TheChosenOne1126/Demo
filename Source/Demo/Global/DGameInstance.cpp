// Fill out your copyright notice in the Description page of Project Settings.

#include "DGameInstance.h"
#include "DAssetManager.h"

void UDGameInstance::Init()
{
	Super::Init();
	
	UDAssetManager& AssetManager = UDAssetManager::Get();
	AssetManager.LoadGameData();
}

FString UDGameInstance::GetModuleName_Implementation() const
{
	return TEXT("GameInstance");
}
