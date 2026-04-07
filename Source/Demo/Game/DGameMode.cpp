// Fill out your copyright notice in the Description page of Project Settings.

#include "DGameMode.h"
#include "Global/Statics.h"
#include "Player/DCharacter.h"

APawn* ADGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	ADCharacter* ResultPawn = GetWorld()->SpawnActor<ADCharacter>(PawnClass, SpawnTransform, SpawnInfo);

	if (!ResultPawn)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("SpawnDefaultPawnAtTransform: Failed to spawn Pawn of type %s at %s"),
			*GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString()));
	}

	ResultPawn->SetPawnTag(DefaultPawnTag);
	return ResultPawn;
}
