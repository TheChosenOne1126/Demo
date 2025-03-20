// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DHUD.generated.h"

class UHUDWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogDHUD, Log, All)

UCLASS(Abstract)
class DEMO_API ADHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "DHUD", meta = (AllowAbstract = "false"))
	TSubclassOf<UHUDWidget> HUDWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UHUDWidget> HUDWidget;
};
