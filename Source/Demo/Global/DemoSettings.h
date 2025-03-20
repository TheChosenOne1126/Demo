// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DemoSettings.generated.h"

class UPawnDataAsset;

UCLASS(DefaultConfig, Config = Game)
class DEMO_API UDemoSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Config, Category = "Damage")
	float ArmorCoe;

	UPROPERTY(EditDefaultsOnly, Config, Category = "PawnData", meta = (AllowAbstract = "false"))
	TSoftObjectPtr<UPawnDataAsset> PawnDataAssetPath;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Ability")
	TSet<int32> GainExtraAbilityPointPawnLevelSet;
};
