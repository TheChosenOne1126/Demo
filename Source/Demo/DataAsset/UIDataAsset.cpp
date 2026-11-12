// Fill out your copyright notice in the Description page of Project Settings.

#include "UIDataAsset.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UUIDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	TSet<FGameplayTag> TagSet;
	for (const FUIType& UIType : UITypes)
	{
		bool bAlreadyExists;
		TagSet.Emplace(UIType.UITag, &bAlreadyExists);
		if (bAlreadyExists)
		{
			Context.AddError(FText::Format(INVTEXT("Duplicate UITag: {1}"),
				FText::FromString(UIType.UITag.ToString())));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	return Result;
}
#endif