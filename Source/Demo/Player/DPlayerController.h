// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "GameFramework/PlayerController.h"
#include "DPlayerController.generated.h"

class UHUDWidget;

UCLASS()
class DEMO_API ADPlayerController : public APlayerController, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
	virtual void ClientRestart_Implementation(APawn* NewPawn) override;
	
	virtual FString GetModuleName_Implementation() const override;
	
protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	void OnClientRestart(APawn* NewPawn);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	void OnPostProcessInput();
};
