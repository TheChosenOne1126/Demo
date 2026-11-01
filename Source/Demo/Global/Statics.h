// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/GameDataAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Statics.generated.h"

class UDAbilitySystemComponent;
class USweepComponent;

DEMO_API DECLARE_LOG_CATEGORY_EXTERN(LogDemoGame, Log, All);

UENUM(BlueprintType)
enum class ELogType : uint8
{
	Verbose,
	Log,
	Warning,
	Error,
	Fatal
};

UCLASS()
class DEMO_API UStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Debug", meta = (WorldContext = "Object"))
	static void Log(const UObject* Object, ELogType Type, const FString& String, bool bDisplayToScreen = true);
	
	UFUNCTION()
	static bool IsShippingBuild();
	
	static UPrimitiveComponent* FindSweptComponent(AActor* Owner);
	
	UFUNCTION(BlueprintPure, Category = "Ability", meta = (DefaultToSelf = "Actor"))
	static UDAbilitySystemComponent* GetAscFromActor(AActor* Actor);
	
	UFUNCTION(BlueprintPure, Category = "Asset")
	static bool GetPawnData(FGameplayTag Tag, FPawnData& PawnData);
};