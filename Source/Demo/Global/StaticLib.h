// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/GameDataAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StaticLib.generated.h"

class UDAbilitySystemComponent;

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
class DEMO_API UStaticLib : public UBlueprintFunctionLibrary
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
	static bool GetPlayerTypeByTag(UPARAM(ref) const FGameplayTag& Tag, FPlayerType& OutPlayerType);
	
	UFUNCTION(BlueprintPure, Category = "Asset")
	static bool GetPawnTypeByTag(UPARAM(ref) const FGameplayTag& Tag, FPawnType& OutPawnType);
	
	UFUNCTION(BlueprintPure, Category = "Asset")
	static bool GetGEClassByTag(UPARAM(ref) const FGameplayTag& Tag, TSubclassOf<UGameplayEffect>& OutGEClass);
	
	UFUNCTION(BlueprintPure, Category = "Asset")
	static bool GetCMClassByTag(UPARAM(ref) const FGameplayTag& Tag, TSubclassOf<UCameraModifier>& OutCMClass);
};