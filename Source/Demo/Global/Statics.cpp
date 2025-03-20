// Fill out your copyright notice in the Description page of Project Settings.

#include "Statics.h"
#include "DAssetManager.h"
#include "GlobalTags.h"
#include "Ability/DGameplayAbility.h"
#include "Component/DAbilitySystemComponent.h"
#include "DataAsset/PawnDataAsset.h"
#include "Interface/SweepInterface.h"
#include "Player/DCharacter.h"

DEMO_API DEFINE_LOG_CATEGORY(LogDemoGame)

static float LogDisplayDuration = 5.f;
static FAutoConsoleVariableRef CVarMaxAbilityTaskCount(
	TEXT("Demo.LogDisplayDuration"),
	LogDisplayDuration,
	TEXT("Global Variable duration to display Log String on screen.")
);

UDAbilitySystemComponent* UStatics::GetDAbilitySystemComponent(const AActor* Owner)
{
	if (!IsValid(Owner))
	{
		Log(Owner, ELogType::Error, FString::Printf(TEXT("%hs: invalid Owner"), __FUNCTION__));
		return nullptr;
	}

	const IAbilitySystemInterface* Asi = Cast<IAbilitySystemInterface>(Owner);
	if (!Asi)
	{
		Log(Owner, ELogType::Error, FString::Printf(TEXT("%hs: Owner is not inherit AbilitySystemInterface"), __FUNCTION__));
		return nullptr;
	}
	
	return Cast<UDAbilitySystemComponent>(Asi->GetAbilitySystemComponent());
}

UAnimMontage* UStatics::GetDeadMontage(const UDGameplayAbility* Ability)
{
	if (!IsValid(Ability))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
		return nullptr;
	}

	const AActor* Avatar = Ability->GetAvatarActorFromActorInfo();
	if (!IsValid(Avatar))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid AvatarActor"), __FUNCTION__));
		return nullptr;
	}

	UClass* AvatarClass = Avatar->GetClass();
	if (!IsValid(AvatarClass))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid AvatarClass"), __FUNCTION__));
		return nullptr;
	}

	const UDAssetManager& DAssetManager = UDAssetManager::GetRef();
	UPawnDataAsset* PawnDataAsset = DAssetManager.GetPawnDataAsset();
	if (!IsValid(PawnDataAsset))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid PawnDataAsset"), __FUNCTION__));
		return nullptr;
	}
	
	FPawnData* PawnDataPtr = PawnDataAsset->PawnDataArr.FindByPredicate(
		[AvatarClass](const FPawnData& PawnData) -> bool
		{
			return AvatarClass->IsChildOf(PawnData.PawnClass);
		});

	if (!PawnDataPtr)
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: nullptr PawnDataPtr"), __FUNCTION__));
		return nullptr;
	}

	return PawnDataPtr->DeadMontage;
}

void UStatics::Log(const UObject* Object, ELogType Type, const FString& String, bool bDisplayToScreen)
{
#if UE_BUILD_SHIPPING && !USE_LOGGING_IN_SHIPPING
	return;
#endif
	
	const UWorld* World = IsValid(GEngine) ? GEngine->GetWorldFromContextObject(Object, EGetWorldErrorMode::ReturnNull) : nullptr;

	FString LogString;
	if (IsValid(World) && World->WorldType == EWorldType::PIE)
	{
		switch (World->GetNetMode())
		{
		case NM_Standalone:
			LogString += TEXT("Standalone");
			break;
		case NM_DedicatedServer:
			LogString += TEXT("DedicatedServer");
			break;
		case NM_ListenServer:
			LogString += TEXT("ListenServer");
			break;
		case NM_Client:
			LogString += FString::Printf(TEXT("Client %d"), UE::GetPlayInEditorID());
			break;
		case NM_MAX:
			break;
		}
	}

	if (IsValid(Object))
	{
		LogString += FString::Printf(TEXT("[%s]: "), *Object->GetName());
	}

	LogString += String;

	FColor Color = FColor::Transparent;
	switch (Type)
	{
	case ELogType::Verbose:
		if (!UE_LOG_ACTIVE(LogDemoGame, Verbose))
		{
			bDisplayToScreen = false;
		}
		else
		{
			Color = FColor::White;
		}
		
		UE_LOG(LogDemoGame, Verbose, TEXT("%s"), *LogString)
		break;
	case ELogType::Log:
		Color = FColor::Green;
		UE_LOG(LogDemoGame, Log, TEXT("%s"), *LogString)
		break;
	case ELogType::Warning:
		Color = FColor::Yellow;
		UE_LOG(LogDemoGame, Warning, TEXT("%s"), *LogString)
		break;
	case ELogType::Error:
		Color = FColor::Red;
		UE_LOG(LogDemoGame, Error, TEXT("%s"), *LogString)
		break;
	case ELogType::Fatal:
		UE_LOG(LogDemoGame, Fatal, TEXT("%s"), *LogString)
		break;
	}

	if (bDisplayToScreen)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, LogDisplayDuration, Color, LogString);
	}
}

void UStatics::MuteInputForAbilityAvatar(const UDGameplayAbility* Ability)
{
	if (!IsValid(Ability))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
		return;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid ActorInfo"), __FUNCTION__));
		return;
	}

	if (!ActorInfo->AvatarActor.IsValid())
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid AvatarActor"), __FUNCTION__));
		return;
	}

	if (!ActorInfo->PlayerController.IsValid())
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid PlayerController"), __FUNCTION__));
		return;
	}

	ActorInfo->AvatarActor->DisableInput(ActorInfo->PlayerController.Get());
}

void UStatics::DestroyAbilityAvatar(const UDGameplayAbility* Ability)
{
	if (!IsValid(Ability))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
		return;
	}

	AActor* AvatarActor = Ability->GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid AvatarActor"), __FUNCTION__));
		return;
	}

	AvatarActor->Destroy();
}

void UStatics::MakeDamageGameplayEffectSpec(
	const UDGameplayAbility* Ability,
	TSubclassOf<UGameplayEffect> DamageEffect,
	FGameplayTagContainer GrantTags,
	FDamageData DamageData,
	FGameplayEffectSpecHandle& DamageEffectSpecHandle)
{
	if (!IsValid(Ability))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
		return;
	}

	DamageEffectSpecHandle = Ability->MakeOutgoingGameplayEffectSpec(DamageEffect, Ability->GetAbilityLevel());
	if (!DamageEffectSpecHandle.IsValid())
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid DamageEffectSpecHandle"), __FUNCTION__));
		return;
	}

	DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(GlobalTags::SetByCallerFixedDamageTag, DamageData.FixedValue);
	DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(GlobalTags::SetByCallerPerHpDamageTag, DamageData.PerHpAttributeValue);
	DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(GlobalTags::SetByCallerPerDamageAttributeTag, DamageData.PerDamageAttributeValue);
	DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(GlobalTags::SetByCallerPerExtraDamageAttributeTag, DamageData.PerExtraDamageAttributeValue);
	DamageEffectSpecHandle.Data->DynamicGrantedTags.AppendTags(GrantTags);
}

void UStatics::TriggerOnHit(const UDGameplayAbility* Ability, FGameplayAbilityTargetDataHandle TargetData)
{
	if (!IsValid(Ability))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
		return;
	}

	for (TSharedPtr Data : TargetData.Data)
	{
		if (!Data.IsValid())
		{
			Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid TargetData"), __FUNCTION__));
			continue;
		}
		
		for (TWeakObjectPtr Actor : Data->GetActors())
		{
			UDAbilitySystemComponent* Asc = GetDAbilitySystemComponent(Actor.Get());
			if (!IsValid(Asc))
			{
				Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: [Actor: %s] invalid UDAbilitySystemComponent"),
					__FUNCTION__, *AActor::GetDebugName(Actor.Get())));
				continue;
			}

			FGameplayEventData EventData;
			EventData.Instigator = Ability->GetAvatarActorFromActorInfo();
			Asc->HandleGameplayEvent(GlobalTags::EventOnHitTag, &EventData);
		}
	}
}

UPrimitiveComponent* UStatics::FindSweptComponent(AActor* Owner)
{
	if (!IsValid(Owner))
	{
		Log(Owner, ELogType::Error, FString::Printf(TEXT("%hs: invalid Owner"), __FUNCTION__));
		return nullptr;
	}

	if (!Owner->Implements<USweepInterface>())
	{
		Log(Owner, ELogType::Error, FString::Printf(TEXT("%hs: Owner is not inherited SweepInterface"), __FUNCTION__));
		return nullptr;
	}

	return ISweepInterface::Execute_GetSweptComponent(Owner);
}