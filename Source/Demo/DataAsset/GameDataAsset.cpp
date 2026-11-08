// Fill out your copyright notice in the Description page of Project Settings.

#include "GameDataAsset.h"

#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UGameDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	TSet<FGameplayTag> TagSet;
	for (const FPawnType& PawnType : PawnTypes)
	{
		bool bAlreadyInSet;
		TagSet.Emplace(PawnType.Tag, &bAlreadyInSet);
		if (bAlreadyInSet)
		{
			Context.AddError(FText::Format(INVTEXT("Duplicate PawnType Tag: {1}"),
				FText::FromString(PawnType.Tag.ToString())));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	return Result;
}
#endif