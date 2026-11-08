// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityDataAsset.generated.h"

class UDGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAbilityType
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = false))
	TSubclassOf<UDGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly)
	int32 OriLv = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxLv = 0;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "InputTag, GA"))
	FGameplayTagContainer Tags;
};

UCLASS()
class DEMO_API UAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
 	UPROPERTY(EditDefaultsOnly)
	TArray<FAbilityType> AbilityTypes;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> OriGEClass;
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
