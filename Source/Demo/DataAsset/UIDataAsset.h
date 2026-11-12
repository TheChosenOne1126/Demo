// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "UIDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FUIType
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "UI.Class"))
	FGameplayTag UITag;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = false))
	TSubclassOf<UUserWidget> WidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	int ZOrder = 0;
};

UCLASS()
class DEMO_API UUIDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FUIType> UITypes;
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
