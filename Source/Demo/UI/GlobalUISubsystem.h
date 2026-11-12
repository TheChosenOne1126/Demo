// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "GlobalUISubsystem.generated.h"

class UGameDataAsset;

UCLASS()
class DEMO_API UGlobalUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCosmetic, BlueprintCallable)
	UUserWidget* CreateUIWidget(UPARAM(meta=(GameplayTagFilter="UI")) FGameplayTag UITag);
	
	UFUNCTION(BlueprintCosmetic, BlueprintCallable)
	void CloseUIWidget(UPARAM(meta=(GameplayTagFilter="UI")) FGameplayTag UITag);
	
protected:
	void OnGameDataLoadedCallback(const UGameDataAsset* GameDataAsset);
	
private:
	FDelegateHandle OnGameDataLoadedHandle;
	
	UPROPERTY()
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> TagToClassMap;
	
	UPROPERTY()
	TMap<FGameplayTag, int> TagToZOrderMap;
	
	UPROPERTY()
	TMap<FGameplayTag, TWeakObjectPtr<UUserWidget>> TagToInstanceMap;
};
