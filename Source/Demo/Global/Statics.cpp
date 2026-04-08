// Fill out your copyright notice in the Description page of Project Settings.

#include "Statics.h"
#include "Interface/SweepInterface.h"

DEMO_API DEFINE_LOG_CATEGORY(LogDemoGame)

static float LogDisplayDuration = 5.f;
static FAutoConsoleVariableRef CVarMaxAbilityTaskCount(
	TEXT("Demo.LogDisplayDuration"),
	LogDisplayDuration,
	TEXT("Global Variable duration to display Log String on screen.")
);

void UStatics::Log(const UObject* Object, ELogType Type, const FString& String, bool bDisplayToScreen)
{
#if UE_BUILD_SHIPPING && !USE_LOGGING_IN_SHIPPING
	return;
#endif
	if (!IsValid(GEngine))
	{
		UE_LOG(LogDemoGame, Fatal, TEXT("%hs: invalid GEngine"), __FUNCTION__)
		return;
	}

	const UWorld* World = GEngine->GetWorldFromContextObject(Object, EGetWorldErrorMode::ReturnNull);
	if (!IsValid(World))
	{
		World = GEngine->GetCurrentPlayWorld();
	}

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

bool UStatics::IsShippingBuild()
{
#if UE_BUILD_SHIPPING
	return true;
#endif
	
	return false;
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
