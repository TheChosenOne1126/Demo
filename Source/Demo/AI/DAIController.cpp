// Fill out your copyright notice in the Description page of Project Settings.

#include "DAIController.h"
#include "Player/DPlayerState.h"

ADAIController::ADAIController()
{
	bWantsPlayerState = true;
}

void ADAIController::InitPlayerState()
{
	if (GetNetMode() != NM_Client)
	{
		UWorld* const World = GetWorld();

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.ObjectFlags |= RF_Transient;
		SetPlayerState(World->SpawnActor<ADPlayerState>(SpawnInfo));

		if (PlayerState && PlayerState->GetPlayerName().IsEmpty())
		{
			const FString InName = FString::Printf(TEXT("%i"), PlayerState->GetPlayerId());
			PlayerState->SetPlayerNameInternal(InName);
		}
	}
}
