// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DemoSettings.generated.h"

class UGameDataAsset;

UCLASS(DefaultConfig, Config = Game)
class DEMO_API UDemoSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Config, Category = "Damage")
	float ArmorCoe;

	UPROPERTY(EditDefaultsOnly, Config, Category = "GameData", meta = (AllowAbstract = "false"))
	TSoftObjectPtr<UGameDataAsset> GameDataAssetPath;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Ability")
	TSet<int32> GainExtraAbilityPointPawnLevelSet;
	
	UPROPERTY(EditDefaultsOnly, Config, Category = "Loading")
	TSoftClassPtr<UUserWidget> LoadingScreenWidgetClassPath;
};
