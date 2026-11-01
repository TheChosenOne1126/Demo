// Fill out your copyright notice in the Description page of Project Settings.

#include "InputDataAsset.h"
#include "InputMappingContext.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UInputDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	if (!ValidateNativeInputTypes(Context))
	{
		Result = EDataValidationResult::Invalid;
	}
	
	if (!ValidateAbilityInputTypes(Context))
	{
		Result = EDataValidationResult::Invalid;
	}
	
	if (!ValidateInputMapTypes(Context))
	{
		Result = EDataValidationResult::Invalid;
	}
	
	return Result;
}

bool UInputDataAsset::ValidateNativeInputTypes(FDataValidationContext& Context) const
{
	bool bResult = true;
	
	TSet<FGameplayTag> TagSet;
	TSet<TObjectPtr<UInputAction>> InputActionSet;
	
	for (const FInputType& InputType : NativeInputTypes)
	{
		bResult = ValidateInputTypes(Context, InputType, TEXT("NativeInputTypes"), TagSet, InputActionSet);
	}
	
	return bResult;
}

bool UInputDataAsset::ValidateAbilityInputTypes(FDataValidationContext& Context) const
{
	bool bResult = true;
	
	TSet<FGameplayTag> TagSet;
	TSet<TObjectPtr<UInputAction>> InputActionSet;
	
	for (const FAbilityInputType& AbilityInputType : AbilityInputTypes)
	{
		bResult = ValidateInputTypes(Context, AbilityInputType, TEXT("AbilityInputTypes"), TagSet, InputActionSet);
	}
	
	return bResult;
}

bool UInputDataAsset::ValidateInputMapTypes(FDataValidationContext& Context) const
{
	bool bResult = true;
	
	TSet<FGameplayTag> TagSet;
	TSet<TObjectPtr<UInputMappingContext>> MappingContextSet;
	
	for (const FInputMapType& InputMapType : InputMapTypes)
	{
		if (!InputMapType.Tag.IsValid())
		{
			Context.AddError(INVTEXT("InputMapTypes Invalid Tag"));
			bResult = false;
		}
	
		bool bIsAlreadyInSet;
		TagSet.Emplace(InputMapType.Tag, &bIsAlreadyInSet);
		if (!bIsAlreadyInSet)
		{
			Context.AddError(FText::Format(INVTEXT("InputMapTypes Duplicate Tag: {1}"),
				FText::FromString(InputMapType.Tag.ToString())));
			bResult = bIsAlreadyInSet;
		}
	
		if (!IsValid(InputMapType.MappingContext))
		{
			Context.AddError(INVTEXT("InputMapTypes Invalid MappingContext"));
			bResult = false;
		}
	
		MappingContextSet.Emplace(InputMapType.MappingContext, &bIsAlreadyInSet);
		if (!bIsAlreadyInSet)
		{
			Context.AddError(FText::Format(INVTEXT("InputMapTypes Duplicate MappingContext: {1}"),
				FText::FromString(GetNameSafe(InputMapType.MappingContext))));
			bResult = false;
		}
	}
	
	return bResult;
}

bool UInputDataAsset::ValidateInputTypes(
	FDataValidationContext& Context,
	const FInputType& InputType,
	const FString& ErrorName,
	TSet<FGameplayTag>& TagSet,
	TSet<TObjectPtr<UInputAction>>& ActionSet)
{
	bool bResult = true;
	
	if (!InputType.InputTag.IsValid())
	{
		Context.AddError(FText::Format(INVTEXT("{0} Invalid Tag"), FText::FromString(ErrorName)));
		bResult = false;
	}
	
	bool bIsAlreadyInSet;
	TagSet.Emplace(InputType.InputTag, &bIsAlreadyInSet);
	if (!bIsAlreadyInSet)
	{
		Context.AddError(FText::Format(INVTEXT("{0} Duplicate Tag: {1}"),
			FText::FromString(ErrorName), FText::FromString(InputType.InputTag.ToString())));
		bResult = bIsAlreadyInSet;
	}
	
	if (!IsValid(InputType.InputAction))
	{
		Context.AddError(FText::Format(INVTEXT("{0} Invalid InputAction"), FText::FromString(ErrorName)));
		bResult = false;
	}
	
	ActionSet.Emplace(InputType.InputAction, &bIsAlreadyInSet);
	if (!bIsAlreadyInSet)
	{
		Context.AddError(FText::Format(INVTEXT("{0} Duplicate InputAction: {1}"),
			FText::FromString(ErrorName), FText::FromString(GetNameSafe(InputType.InputAction))));
		bResult = false;
	}
	
	return bResult;
}
#endif
