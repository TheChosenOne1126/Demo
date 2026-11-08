// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GameDataAsset.generated.h"

class UGameplayEffect;
class UMontageDataAsset;
class UInputDataAsset;
class UAbilityDataAsset;
class ADCharacter;

USTRUCT(BlueprintType)
struct FPawnType
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Pawn"))
	FGameplayTag Tag;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
	TSubclassOf<ADCharacter> PawnClass;

	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
	TObjectPtr<UAbilityDataAsset> AbilityDataAsset;

	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
	TObjectPtr<UMontageDataAsset> MontageDataAsset;
};

USTRUCT()
struct FPlayerType : public FPawnType
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
	TObjectPtr<UInputDataAsset> InputDataAsset;
};

USTRUCT(BlueprintType)
struct FGameplayEffectType
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Tag;
};

USTRUCT(BlueprintType)
struct FCameraModifierType
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "CameraModifier"))
	FGameplayTag Tag;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
	TSubclassOf<UCameraModifier> CameraModifierClass;
};

UCLASS()
class DEMO_API UGameDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FPawnType> PawnTypes;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FPlayerType> PlayerTypes;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayEffectType> GameplayEffectTypes;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FCameraModifierType> CameraModifierTypes;
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};