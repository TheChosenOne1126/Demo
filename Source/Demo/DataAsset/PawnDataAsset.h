// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "PawnDataAsset.generated.h"

class UGameplayEffect;
class UMontageDataAsset;
class UInputDataAsset;
class UAbilityDataAsset;
class ADCharacter;

USTRUCT(BlueprintType)
struct FPawnData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
	TSubclassOf<ADCharacter> PawnClass;

	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
	TObjectPtr<UAbilityDataAsset> AbilityDataAsset;

	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
	TObjectPtr<UInputDataAsset> InputDataAsset;

	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
	TObjectPtr<UMontageDataAsset> MontageDataAsset;
};

USTRUCT(BlueprintType)
struct FGameplayEffectData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Tag;
};

UCLASS()
class DEMO_API UPawnDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FPawnData> PawnDataMap;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> GameplayEffectMap;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, TSubclassOf<UCameraModifier>> CameraModifierMap;
};