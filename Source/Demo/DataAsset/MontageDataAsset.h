// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MontageDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FMontageData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag MontageTag;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> Montage;
};

UCLASS()
class DEMO_API UMontageDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> MontageDataMap;
};
