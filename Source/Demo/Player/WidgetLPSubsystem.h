// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "WidgetLPSubsystem.generated.h"

class UGameDataAsset;

UCLASS()
class DEMO_API UWidgetLPSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCosmetic, BlueprintCallable)
	UUserWidget* CreateUIWidget(UPARAM(meta=(GameplayTagFilter="UI")) FGameplayTag UITag);
	
	UFUNCTION(BlueprintCosmetic, BlueprintCallable)
	void CloseUIWidget(UPARAM(meta=(GameplayTagFilter="UI")) FGameplayTag UITag);
	
private:
	UPROPERTY(Transient)
	TMap<FGameplayTag, TWeakObjectPtr<UUserWidget>> TagToInstanceMap;
};
