// Fill out your copyright notice in the Description page of Project Settings.

#include "DCharacter.h"
#include "Ability/DGameplayAbility.h"
#include "Ability/DAbilitySystemComponent.h"
#include "DataAsset/AbilityDataAsset.h"
#include "DataAsset/GameDataAsset.h"
#include "GameFramework/PlayerState.h"
#include "Global/GlobalTags.h"
#include "Global/StaticLib.h"
#include "Interface/PawnTagInterface.h"

void ADCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
	{
		return;
	}
	
	const IPawnTagInterface* PawnTagInterface = Cast<IPawnTagInterface>(NewController);
	if (!PawnTagInterface)
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid PawnTagInterface"), __FUNCTION__));
		return;
	}

	const FGameplayTag Tag = PawnTagInterface->GetPawnTag();

	APlayerState* PS = GetPlayerState();
	if (!IsValid(PS))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid PlayerState"), __FUNCTION__));
		return;
	}
	
	UDAbilitySystemComponent* Asc = UStaticLib::GetAscFromActor(PS);
	if (!IsValid(Asc))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}
	Asc->InitAbilityActorInfo(PS, this);
	
	UAbilityDataAsset* AbilityDataAsset = nullptr;
	if (PS->IsABot())
	{
		FPawnType PawnData;
		if (!UStaticLib::GetPawnTypeByTag(Tag, PawnData))
		{
			UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid PawnData"), __FUNCTION__));
			return;
		}
		
		AbilityDataAsset = PawnData.AbilityDataAsset;
	}
	else
	{
		FPlayerType PlayerData;
		if (!UStaticLib::GetPlayerTypeByTag(Tag, PlayerData))
		{
			UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid PlayerData"), __FUNCTION__));
			return;
		}
		
		AbilityDataAsset = PlayerData.AbilityDataAsset;
	}
	
	if (!IsValid(AbilityDataAsset))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid AbilityDataAsset"), __FUNCTION__));
		return;
	}

	for (const FAbilityType& AbilityType : AbilityDataAsset->AbilityTypes)
	{
		if (!IsValid(AbilityType.AbilityClass))
		{
			UStaticLib::Log(this, ELogType::Error, FString::Printf(
				TEXT("%hs: Invalid AbilityClass, Tag: [%s]"), __FUNCTION__, *AbilityType.Tags.ToString()));
			continue;
		}
		
		FGameplayAbilitySpec Spec(AbilityType.AbilityClass, AbilityType.OriLv);
		Spec.SetByCallerTagMagnitudes.Emplace(GlobalTags::SETBYCALLER_GA_MAXLV, AbilityType.MaxLv);
		Spec.GetDynamicSpecSourceTags().AppendTags(AbilityType.Tags);
		
		Asc->GiveAbility(Spec);
	}

	if (!IsValid(AbilityDataAsset->OriGEClass))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid OriGEClass"), __FUNCTION__));
		return;
	}
	
	FGameplayEffectSpecHandle Handle = Asc->MakeOutgoingSpec(AbilityDataAsset->OriGEClass, 1.f, Asc->MakeEffectContext());
	Asc->ApplyGameplayEffectSpecToSelf(*Handle.Data);
}

void ADCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	UDAbilitySystemComponent* Asc = UStaticLib::GetAscFromActor(GetPlayerState());
	if (!IsValid(Asc))
	{
		UStaticLib::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}
	
	Asc->InitAbilityActorInfo(GetPlayerState(), this);
}
