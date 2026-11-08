// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityDataAsset.h"
#include "Ability/DGameplayAbility.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UAbilityDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	TSet<TSubclassOf<UDGameplayAbility>> AbilitySet;
	for (const FAbilityType& AbilityType : AbilityTypes)
	{
		bool bIsAlreadyInSet;
		AbilitySet.Emplace(AbilityType.AbilityClass, &bIsAlreadyInSet);
		if (bIsAlreadyInSet)
		{
			Context.AddError(FText::Format(INVTEXT("Duplicate AbilityClass: {1}"),
				FText::FromString(GetNameSafe(AbilityType.AbilityClass))));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	return Result;
}
#endif