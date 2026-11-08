// Fill out your copyright notice in the Description page of Project Settings.

#include "DCharacter.h"
#include "Ability/DGameplayAbility.h"
#include "Component/DAbilitySystemComponent.h"
#include "DataAsset/AbilityDataAsset.h"
#include "DataAsset/GameDataAsset.h"
#include "GameFramework/PlayerState.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"
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
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid PawnTagInterface"), __FUNCTION__));
		return;
	}

	const FGameplayTag Tag = PawnTagInterface->GetPawnTag();

	APlayerState* PS = GetPlayerState();
	if (!IsValid(PS))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid PlayerState"), __FUNCTION__));
		return;
	}
	
	UDAbilitySystemComponent* Asc = UStatics::GetAscFromActor(PS);
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}
	Asc->InitAbilityActorInfo(PS, this);
	
	UAbilityDataAsset* AbilityDataAsset = nullptr;
	if (PS->IsABot())
	{
		FPawnType PawnData;
		if (!UStatics::GetPawnType(Tag, PawnData))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid PawnData"), __FUNCTION__));
			return;
		}
		
		AbilityDataAsset = PawnData.AbilityDataAsset;
	}
	else
	{
		FPlayerType PlayerData;
		if (!UStatics::GetPlayerType(Tag, PlayerData))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid PlayerData"), __FUNCTION__));
			return;
		}
		
		AbilityDataAsset = PlayerData.AbilityDataAsset;
	}
	
	if (!IsValid(AbilityDataAsset))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid AbilityDataAsset"), __FUNCTION__));
		return;
	}

	for (const FAbilityType& AbilityType : AbilityDataAsset->AbilityTypes)
	{
		if (!IsValid(AbilityType.AbilityClass))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(
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
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid OriGEClass"), __FUNCTION__));
		return;
	}
	
	FGameplayEffectSpecHandle Handle = Asc->MakeOutgoingSpec(AbilityDataAsset->OriGEClass, 1.f, Asc->MakeEffectContext());
	Asc->ApplyGameplayEffectSpecToSelf(*Handle.Data);
}

void ADCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	UDAbilitySystemComponent* Asc = UStatics::GetAscFromActor(GetPlayerState());
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}
	
	Asc->InitAbilityActorInfo(GetPlayerState(), this);
}
