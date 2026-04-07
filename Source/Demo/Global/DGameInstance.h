// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "Engine/GameInstance.h"
#include "DGameInstance.generated.h"

UCLASS()
class DEMO_API UDGameInstance : public UGameInstance, public IUnLuaInterface
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
	virtual FString GetModuleName_Implementation() const override;
	
	UPROPERTY(Transient)
	TSubclassOf<UUserWidget> LoadingScreenClass;
};
