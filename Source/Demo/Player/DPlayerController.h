// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/AbilityDataAsset.h"
#include "GameFramework/PlayerController.h"
#include "DPlayerController.generated.h"

class UHUDWidget;

UCLASS()
class DEMO_API ADPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	void CreateHUD(const TArray<FAbilityData>& SlotAbilityDataArr);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HUD", meta = (AllowAbstract = "false"))
	TSoftClassPtr<UHUDWidget> HUDWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UHUDWidget> HUDWidget;
};
