// Fill out your copyright notice in the Description page of Project Settings.

#include "DGameInstance.h"
#include "DemoSettings.h"
#include "Statics.h"
#include "Blueprint/UserWidget.h"

void UDGameInstance::Init()
{
	Super::Init();

	if (!IsDedicatedServerInstance())
	{
		const UDemoSettings* DemoSettings = GetDefault<UDemoSettings>();
		if (!IsValid(DemoSettings))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Demo Settings"), __FUNCTION__));
			return;
		}

		if (DemoSettings->LoadingScreenWidgetClassPath.IsNull())
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: null LoadingScreenWidgetClassPath"), __FUNCTION__));
			return;
		}
	
		LoadingScreenClass = DemoSettings->LoadingScreenWidgetClassPath.LoadSynchronous();
	}
}

FString UDGameInstance::GetModuleName_Implementation() const
{
	return TEXT("GameInstance");
}
