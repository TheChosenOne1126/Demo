// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "InputDataAsset.generated.h"

class UInputMappingContext;

USTRUCT(BlueprintType)
struct FInputType
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction;
};

USTRUCT(BlueprintType)
struct FAbilityInputType : public FInputType
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ActivationTag;
};

USTRUCT(BlueprintType)
struct FInputMapType
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Tag;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly)
	int32 Priority = 0;
};

UCLASS()
class DEMO_API UInputDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FInputType> NativeInputTypes;

	UPROPERTY(EditDefaultsOnly)
	TArray<FAbilityInputType> AbilityInputTypes;

	UPROPERTY(EditDefaultsOnly)
	TArray<FInputMapType> InputMapTypes;
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	
private:
#if WITH_EDITOR
	bool ValidateNativeInputTypes(FDataValidationContext& Context) const;
	
	bool ValidateAbilityInputTypes(FDataValidationContext& Context) const;
	
	bool ValidateInputMapTypes(FDataValidationContext& Context) const;

	static bool ValidateInputTypes(
		FDataValidationContext& Context,
		const FInputType& InputType,
		const FString& ErrorName,
		TSet<FGameplayTag>& TagSet,
		TSet<TObjectPtr<UInputAction>>& ActionSet);
#endif
};