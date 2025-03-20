// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayAbilitySpec.h"
#include "Global/MessageStructTypes.h"
#include "Global/MessageSubsystem.h"
#include "AbilitySlotWidget.generated.h"

class UCommonButtonBase;
class UCommonNumericTextBlock;
class UImage;

UCLASS(Abstract)
class DEMO_API UAbilitySlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	
	FGameplayAbilitySpec* GetAbilitySpecByHandle() const;

	void OnAbilityGiven(const FGameplayAbilitySpecHandle& SpecHandle);

	void OnAbilityLevelUpdated(const FGameplayAbilitySpecHandle& SpecHandle);
	
	void OnAttributeBaseSpChanged(const FMessageFloatStructType& MessageStruct);
	
	void OnAttributeUltimateSpChanged(const FMessageFloatStructType& MessageStruct);

	void OnAbilityLevelUpdateButtonClicked();
	
	UPROPERTY(EditInstanceOnly, Category = "AbilitySlot")
	FGameplayTag AbilitySlotTag;

private:
	void OnAttributeAbilityPointChanged(float AbilityPointValue, const FGameplayAbilitySpec* AbilitySpec);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AbilityImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonNumericTextBlock> AbilityLevelText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> AbilityLevelUpdateButton;

	UPROPERTY()
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	TMap<FGameplayTag, FGuid> MessageHandleMap;

	FDelegateHandle AbilityUpdateLevelClickDelegateHandle;
};
