// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PawnDataAsset.generated.h"

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

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> DeadMontage;
};

UCLASS()
class DEMO_API UPawnDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FPawnData> PawnDataArr;
};
