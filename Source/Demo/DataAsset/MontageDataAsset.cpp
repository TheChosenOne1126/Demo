// Fill out your copyright notice in the Description page of Project Settings.

#include "MontageDataAsset.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UMontageDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	TSet<FGameplayTag> TagSet;
	for (const FMontageType& MontageType : MontageTypes)
	{
		bool bAlreadyExists;
		TagSet.Emplace(MontageType.MontageTag, &bAlreadyExists);
		if (bAlreadyExists)
		{
			Context.AddError(FText::Format(INVTEXT("Duplicate MontageTag: {1}"),
				FText::FromString(MontageType.MontageTag.ToString())));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	return Result;
}
#endif