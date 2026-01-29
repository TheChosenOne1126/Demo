// Fill out your copyright notice in the Description page of Project Settings.

#include "Statics.h"
#include "DAssetManager.h"
#include "GlobalTags.h"
#include "Ability/DGameplayAbility.h"
#include "Component/DAbilitySystemComponent.h"
#include "DataAsset/MontageDataAsset.h"
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

UAnimMontage* UStatics::GetMontageByTag(const UDGameplayAbility* Ability, FGameplayTag MontageTag)
{
	if (!MontageTag.IsValid())
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid MontageTag"), __FUNCTION__));
		return nullptr;
	}
	
	if (!IsValid(Ability))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
		return nullptr;
	}

	const ADCharacter* Avatar = Cast<ADCharacter>(Ability->GetAvatarActorFromActorInfo());
	if (!IsValid(Avatar))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid AvatarActor"), __FUNCTION__));
		return nullptr;
	}

	const UDAssetManager& DAssetManager = UDAssetManager::Get();
	UPawnDataAsset* PawnDataAsset = DAssetManager.GetPawnDataAsset();
	if (!IsValid(PawnDataAsset))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid PawnDataAsset"), __FUNCTION__));
		return nullptr;
	}
	
	if (!PawnDataAsset->PawnDataMap.Contains(Avatar->GetPawnTag()))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: PawnDataMap no contains Avatar Tag:[%s]"), __FUNCTION__, *Avatar->GetPawnTag().ToString()));
		return nullptr;
	}
	
	UMontageDataAsset* MontageDataAsset = PawnDataAsset->PawnDataMap[Avatar->GetPawnTag()].MontageDataAsset;
	if (!IsValid(MontageDataAsset))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: invalid MontageDataAsset"), __FUNCTION__));
		return nullptr;
	}
	
	if (!MontageDataAsset->MontageDataMap.Contains(MontageTag))
	{
		Log(Ability, ELogType::Error, FString::Printf(TEXT("%hs: Tag[%s] in MontageDataMap"), __FUNCTION__, *MontageTag.ToString()));
		return nullptr;
	}
	
	return MontageDataAsset->MontageDataMap[MontageTag];
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

FGameplayTag UStatics::RequestGameplayTag(const FName TagName)
{
	return UGameplayTagsManager::Get().RequestGameplayTag(TagName, true);
}

TSubclassOf<UGameplayEffect> UStatics::GetGameplayEffectByTag(FGameplayTag EffectTag)
{
	UPawnDataAsset* PawnDataAsset = UDAssetManager::Get().GetPawnDataAsset();
	if (!IsValid(PawnDataAsset))
	{
		Log(&UDAssetManager::Get(), ELogType::Error, FString::Printf(TEXT("%hs: invalid Owner"), __FUNCTION__));
		return nullptr;
	}
	
	const int32 Index = PawnDataAsset->GameplayEffectDataList.IndexOfByPredicate(
		[EffectTag](const FGameplayEffectData& EffectData) -> bool
		{
			return EffectData.Tag.MatchesTagExact(EffectTag);
		});
	
	if (Index == INDEX_NONE)
	{
		Log(&UDAssetManager::Get(), ELogType::Error, FString::Printf(TEXT("%hs: none Index in MontageDataList"), __FUNCTION__));
		return nullptr;
	}
	
	return PawnDataAsset->GameplayEffectDataList[Index].GameplayEffectClass;
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