// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/DGameplayAbility.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Statics.generated.h"

class USweepComponent;
class UGameplayEffect;
class UDGameplayAbility;
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

USTRUCT(BlueprintType)
struct FDamageData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	float FixedValue = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float PerHpAttributeValue = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float PerDamageAttributeValue = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float PerExtraDamageAttributeValue = 0.f;
};

UCLASS()
class DEMO_API UStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Ability", meta = (DefaultToSelf = "Owner"))
	static UDAbilitySystemComponent* GetDAbilitySystemComponent(const AActor* Owner);

	UFUNCTION(BlueprintPure, Category = "Ability", meta = (DefaultToSelf = "Ability"))
	static UAnimMontage* GetDeadMontage(const UDGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "Debug", meta = (WorldContext = "Object"))
	static void Log(const UObject* Object, ELogType Type, const FString& String, bool bDisplayToScreen = true);

	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (DefaultToSelf = "Ability"))
	static void MuteInputForAbilityAvatar(const UDGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (DefaultToSelf = "Ability"))
	static void DestroyAbilityAvatar(const UDGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (DefaultToSelf = "Ability"))
	static void MakeDamageGameplayEffectSpec(
		const UDGameplayAbility* Ability,
		TSubclassOf<UGameplayEffect> DamageEffect,
		FGameplayTagContainer GrantTags,
		FDamageData DamageData,
		FGameplayEffectSpecHandle& DamageEffectSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (DefaultToSelf = "Ability"))
	static void TriggerOnHit(const UDGameplayAbility* Ability, FGameplayAbilityTargetDataHandle TargetData);
	
	static UPrimitiveComponent* FindSweptComponent(AActor* Owner);
};
