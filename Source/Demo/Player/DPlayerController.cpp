// Fill out your copyright notice in the Description page of Project Settings.

#include "DPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Component/DAbilitySystemComponent.h"
#include "DataAsset/InputDataAsset.h"
#include "GameFramework/PlayerState.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

void ADPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);
	
	if (bGamePaused)
	{
		return;
	}
	
	UDAbilitySystemComponent* Asc = UStatics::GetAscFromActor(PlayerState);
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid Asc"), __FUNCTION__));
		return;
	}
	
	Asc->ProcessAbilityInput();
}

FString ADPlayerController::GetModuleName_Implementation() const
{
	return TEXT("Player.PlayerController");
}

void ADPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PlayerTag, Params);
}

void ADPlayerController::OnRep_PlayerTag()
{
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EIC))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid UEnhancedInputComponent"), __FUNCTION__));
		return;
	}
	
	if (!PlayerTag.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid PlayerTag"), __FUNCTION__));
		return;
	}
	
	FPlayerType PlayerData;
	if (!UStatics::GetPlayerType(PlayerTag, PlayerData))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid PawnData"), __FUNCTION__));
		return;
	}
	
	if (!IsValid(PlayerData.InputDataAsset))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid InputDataAsset"), __FUNCTION__));
		return;
	}

	for (const FInputType& InputType : PlayerData.InputDataAsset->NativeInputTypes)
	{
		const FGameplayTag& Tag = InputType.InputTag;
		if (!Tag.IsValid())
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputType Invalid InputTag"), __FUNCTION__));
			continue;
		}

		const UInputAction* Action = InputType.InputAction;
		if (!IsValid(Action))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputType Invalid InputAction"), __FUNCTION__));
			continue;
		}
		
		EIC->BindActionValueLambda(Action, ETriggerEvent::Triggered,
			[this, Tag](const FInputActionValue& Value) -> void
			{
				OnNativeInput(Value, Tag);
			});
	}

	for (const FAbilityInputType& AbilityInputType : PlayerData.InputDataAsset->AbilityInputTypes)
	{
		const FGameplayTag& Tag = AbilityInputType.InputTag;
		if (!Tag.IsValid())
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: AbilityInput Invalid InputTag"), __FUNCTION__));
			continue;
		}

		if (!IsValid(AbilityInputType.InputAction))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: AbilityInput Invalid InputAction"), __FUNCTION__));
			continue;
		}
		
		EIC->BindActionValueLambda(AbilityInputType.InputAction, ETriggerEvent::Triggered,
			[this, Tag](const FInputActionValue&) -> void
			{
				OnAbilityTrigger(Tag);
			});

		if (AbilityInputType.InputTriggerTag.MatchesTagExact(GlobalTags::GA_INPUT_HELD))
		{
			EIC->BindActionValueLambda(AbilityInputType.InputAction, ETriggerEvent::Started,
				[this, Tag](const FInputActionValue&) -> void
				{
					OnAbilityStart(Tag);
				});
			
			EIC->BindActionValueLambda(AbilityInputType.InputAction, ETriggerEvent::Canceled,
				[this, Tag](const FInputActionValue&) -> void
				{
					OnAbilityCancel(Tag);
				});
		}
	}
}

FGameplayTag ADPlayerController::GetPawnTag() const
{
	return PlayerTag;
}

void ADPlayerController::SetPawnTag(const FGameplayTag& NewPawnTag)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (PlayerTag.IsValid())
	{
		return;
	}
	
	PlayerTag = NewPawnTag;

	if (GetGameInstance()->IsDedicatedServerInstance())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ADPlayerController, PlayerTag, this);
	}
	else
	{
		OnRep_PlayerTag();
	}
}

void ADPlayerController::ServerSetPlayerTag_Implementation(const FGameplayTag& NewPlayerTag)
{
	SetPawnTag(NewPlayerTag);
}

bool ADPlayerController::ServerSetPlayerTag_Validate(const FGameplayTag& NewPlayerTag)
{
	return true;
}

void ADPlayerController::OnNativeInput(const FInputActionValue& Value, const FGameplayTag& Tag)
{
	if (Tag.MatchesTagExact(GlobalTags::IATAG_MOVE))
	{
		OnMoveInput(Value);
	}
	else if (Tag.MatchesTagExact(GlobalTags::IATAG_LOOKMOUSE))
	{
		OnLookInput(Value);
	}
}

void ADPlayerController::OnMoveInput(const FInputActionValue& Value) const
{
	APawn* P = GetPawn();
	if (!IsValid(P))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid Pawn"), __FUNCTION__));
		return;
	}

	const FRotator MoveRotator = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	const FVector2D InputVector = Value.Get<FVector2D>();
	
	if (InputVector.X != 0.f)
	{
		P->AddMovementInput(MoveRotator.RotateVector(FVector(0.f, 1.f, 0.f)), InputVector.X);
	}

	if (InputVector.Y != 0.f)
	{
		P->AddMovementInput(MoveRotator.RotateVector(FVector(1.f, 0.f, 0.f)), InputVector.Y);
	}
}

void ADPlayerController::OnLookInput(const FInputActionValue& Value)
{
	const FVector2D InputVector = Value.Get<FVector2D>();
	
	if (InputVector.X != 0.f)
	{
		AddYawInput(InputVector.X);
	}
	
	if (InputVector.Y != 0.f)
	{
		AddPitchInput(InputVector.Y);
	}
}

void ADPlayerController::OnAbilityTrigger(const FGameplayTag& Tag) const
{
	UDAbilitySystemComponent* Asc = UStatics::GetAscFromActor(PlayerState);
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	
	Asc->AbilityForInputTrigger(Tag);
}

void ADPlayerController::OnAbilityStart(const FGameplayTag& Tag) const
{
	UDAbilitySystemComponent* Asc = UStatics::GetAscFromActor(PlayerState);
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	Asc->AbilityForInputStart(Tag);
}

void ADPlayerController::OnAbilityCancel(const FGameplayTag& Tag) const
{
	UDAbilitySystemComponent* Asc = UStatics::GetAscFromActor(PlayerState);
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Invalid UDAbilitySystemComponent"), __FUNCTION__));
		return;
	}
	
	Asc->AbilityForInputCancel(Tag);
}

void ADPlayerController::ClientPostLogin_Implementation()
{
	K2_OnClientPostLogin();
}
