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

UCLASS()
class DEMO_API UStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Ability", meta = (DefaultToSelf = "Owner"))
	static UDAbilitySystemComponent* GetDAbilitySystemComponent(const AActor* Owner);

	UFUNCTION(BlueprintPure, Category = "Ability", meta = (DefaultToSelf = "Ability"))
	static UAnimMontage* GetMontageByTag(const UDGameplayAbility* Ability, FGameplayTag MontageTag);

	UFUNCTION(BlueprintCallable, Category = "Debug", meta = (WorldContext = "Object"))
	static void Log(const UObject* Object, ELogType Type, const FString& String, bool bDisplayToScreen = true);

	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (DefaultToSelf = "Ability"))
	static void MuteInputForAbilityAvatar(const UDGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (DefaultToSelf = "Ability"))
	static void DestroyAbilityAvatar(const UDGameplayAbility* Ability);
	
	UFUNCTION(BlueprintCallable, Category = "Tag")
	static FGameplayTag RequestGameplayTag(const FName TagName);
	
	UFUNCTION(BlueprintCallable, Category = "Effect", meta = (DefaultToSelf = "Ability"))
	static TSubclassOf<UGameplayEffect> GetGameplayEffectByTag(FGameplayTag EffectTag);
	
	static UPrimitiveComponent* FindSweptComponent(AActor* Owner);
	
#if WITH_EDITOR
	template <typename KeyType, typename DataType, typename ValidFunc, typename ToStringFunc, typename ToKeyFunc>
	static int32 CheckDataAssetKeyUnique(const TArray<DataType>& DataList, ValidFunc Valid, ToStringFunc ToString, ToKeyFunc ToKey);
#endif
};

#if WITH_EDITOR
template <typename KeyType,typename DataType, typename ValidFunc, typename ToStringFunc, typename ToKeyFunc>
int32 UStatics::CheckDataAssetKeyUnique(const TArray<DataType>& DataList, ValidFunc Valid, ToStringFunc ToString, ToKeyFunc ToKey)
{
	TArray<KeyType> Keys;
	for (DataType Data : DataList)
	{
		Keys.Add(ToKey(Data));
	}
	
	TSet<KeyType> UsedKeys;
	int32 DuplicatedIndex = INDEX_NONE;
	for (int32 i = 0; i < Keys.Num(); ++i)
	{
		if (!Valid(Keys[i]))
		{
			continue;
		}

		if (UsedKeys.Contains(Keys[i]))
		{
			DuplicatedIndex = i;
			break;
		}
		
		UsedKeys.Add(Keys[i]);
	}

	if (DuplicatedIndex != INDEX_NONE)
	{
		const FString DuplicatedKeyString = TEXT("Duplicate Key: " + ToString(Keys[DuplicatedIndex]));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(DuplicatedKeyString));
	}
	
	return DuplicatedIndex;
}

#define CHECK_DATA_ASSET_KEY_UNIQUE(DataList, KeyType, Valid, ToString, ToKey) \
	const int32 Index = UStatics::CheckDataAssetKeyUnique<KeyType>(DataList, Valid, ToString, ToKey); \
	if (DataList.IsValidIndex(Index)) \
	{ \
		DataList = Cache##DataList; \
	}
#endif