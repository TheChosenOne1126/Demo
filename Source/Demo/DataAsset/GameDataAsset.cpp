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
			Context.AddError(FText::Format(INVTEXT("Duplicate PawnType Tag: {0}"),
				FText::FromString(PawnType.Tag.ToString())));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	TagSet.Reset();
	for (const FPlayerType& PlayerType : PlayerTypes)
	{
		bool bAlreadyInSet;
		TagSet.Emplace(PlayerType.Tag, &bAlreadyInSet);
		if (bAlreadyInSet)
		{
			Context.AddError(FText::Format(INVTEXT("Duplicate PlayerType Tag: {0}"),
				FText::FromString(PlayerType.Tag.ToString())));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	TagSet.Reset();
	for (const FGameplayEffectType& GameplayEffectType : GameplayEffectTypes)
	{
		bool bAlreadyInSet;
		TagSet.Emplace(GameplayEffectType.Tag, &bAlreadyInSet);
		if (bAlreadyInSet)
		{
			Context.AddError(FText::Format(INVTEXT("Duplicate GameplayEffectType Tag: {0}"),
				FText::FromString(GameplayEffectType.Tag.ToString())));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	TagSet.Reset();
	for (const FCameraModifierType& CameraModifierType : CameraModifierTypes)
	{
		bool bAlreadyInSet;
		TagSet.Emplace(CameraModifierType.Tag, &bAlreadyInSet);
		if (bAlreadyInSet)
		{
			Context.AddError(FText::Format(INVTEXT("Duplicate CameraModifierType Tag: {0}"),
				FText::FromString(CameraModifierType.Tag.ToString())));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	return Result;
}
#endif