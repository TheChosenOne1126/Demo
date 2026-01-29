// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "GlobalUISubsystem.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UGlobalUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCosmetic)
	void SetUpLoadingScreenWidget(TSubclassOf<UUserWidget> LoadingScreenWidgetClass);
	
	UFUNCTION(BlueprintCosmetic)
	void CloseLoadingScreenWidget();
	
private:
	TSharedPtr<SWidget> LoadingScreenWidget;
};
