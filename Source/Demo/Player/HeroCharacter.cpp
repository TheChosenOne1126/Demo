// Fill out your copyright notice in the Description page of Project Settings.

#include "HeroCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Component/DAbilitySystemComponent.h"
#include "DataAsset/InputDataAsset.h"
#include "DataAsset/PawnDataAsset.h"
#include "Global/DAssetManager.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"
#include "Kismet/KismetSystemLibrary.h"

void AHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const UDAssetManager& AssetManager = UDAssetManager::GetRef();

	UPawnDataAsset* PawnDataAsset = AssetManager.GetPawnDataAsset();
	if (!IsValid(PawnDataAsset))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid PawnDataAsset"), __FUNCTION__));
		return;
	}

	const int32 Index = PawnDataAsset->PawnDataArr.IndexOfByPredicate(
		[this](const FPawnData& PawnData) -> bool
		{
			return PawnData.PawnClass == this->GetClass();
		});

	if (Index == INDEX_NONE)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: none Index in PawnDataArr"), __FUNCTION__));
		return;
	}

	const UInputDataAsset* InputDataAsset = PawnDataAsset->PawnDataArr[Index].InputDataAsset;
	if (!IsValid(InputDataAsset))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid UInputDataAsset"), __FUNCTION__));
		return;
	}

	const APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid APlayerController"), __FUNCTION__));
		return;
	}

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid ULocalPlayer"), __FUNCTION__));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!IsValid(EnhancedInputLocalPlayerSubsystem))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid EnhancedInputLocalPlayerSubsystem"), __FUNCTION__));
		return;
	}

	const FInputMappingData& InputMappingData = InputDataAsset->InputMappingData;
	if (!IsValid(InputMappingData.MappingContext))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid InputMappingContext"), __FUNCTION__));
		return;
	}

	EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputMappingData.MappingContext, InputMappingData.Priority);

	BindInputActions(PlayerInputComponent, InputDataAsset);
}

void AHeroCharacter::DestroyPlayerInputComponent()
{
	if (UKismetSystemLibrary::IsDedicatedServer(this))
	{
		return;
	}
	
	UEnhancedInputComponent* DEnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(DEnhancedInputComp))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: UDEnhancedInputComponent is invalid"), __FUNCTION__));
		return;
	}

	if (!InputForMoveHandle.IsSet())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputForMoveHandle is not Set"), __FUNCTION__));
	}
	else
	{
		DEnhancedInputComp->RemoveBindingByHandle(InputForMoveHandle.GetValue());
		InputForMoveHandle.Reset();
	}

	if (!InputForLookMouseHandle.IsSet())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: InputForLookMouseHandle is not Set"), __FUNCTION__));
	}
	else
	{
		DEnhancedInputComp->RemoveBindingByHandle(InputForLookMouseHandle.GetValue());
		InputForLookMouseHandle.Reset();
	}

	for (const uint32& InputForAbilityHandle : InputForAbilityHandles)
	{
		DEnhancedInputComp->RemoveBindingByHandle(InputForAbilityHandle);
	}
	InputForAbilityHandles.Reset();
	
	Super::DestroyPlayerInputComponent();
}

void AHeroCharacter::BindInputActions(UInputComponent* PlayerInputComponent, const UInputDataAsset* InputDataAsset)
{
	UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInputComp))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid EnhancedInputComponent"), __FUNCTION__));
		return;
	}
	
	const int32 InputMoveIndex = InputDataAsset->NativeInputs.IndexOfByPredicate(
		[](const FInputData& InputData) -> bool
		{
			return InputData.Tag.MatchesTagExact(GlobalTags::InputMoveTag);
		});
	
	if (InputMoveIndex == INDEX_NONE)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: none Input Move Index in NativeInputs"), __FUNCTION__));
		return;
	}

	const UInputAction* InputMoveAction = InputDataAsset->NativeInputs[InputMoveIndex].Action;
	if (!IsValid(InputMoveAction))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Native InputDataForMove Action"), __FUNCTION__));
		return;
	}
	
	InputForMoveHandle = EnhancedInputComp->BindAction(InputMoveAction,	ETriggerEvent::Triggered, this, &AHeroCharacter::InputForMove).GetHandle();

	const int32 InputLookMouseIndex = InputDataAsset->NativeInputs.IndexOfByPredicate(
		[](const FInputData& InputData) -> bool
		{
			return InputData.Tag.MatchesTagExact(GlobalTags::InputLookMouseTag);
		});

	if (InputLookMouseIndex == INDEX_NONE)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: none Input Look Mouse Index in NativeInputs"), __FUNCTION__));
		return;
	}

	const UInputAction* InputLookMouseAction = InputDataAsset->NativeInputs[InputLookMouseIndex].Action;
	if (!IsValid(InputLookMouseAction))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Native InputDataForLookMouse Action"), __FUNCTION__));
		return;
	}

	InputForLookMouseHandle = EnhancedInputComp->BindAction(InputLookMouseAction, ETriggerEvent::Triggered, this, &AHeroCharacter::InputForLookMouseUp).GetHandle();

	for (const FInputData& InputDataForAbility : InputDataAsset->AbilityInputs)
	{
		if (!InputDataForAbility.Tag.IsValid())
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid InputDataForAbility Tag"), __FUNCTION__));
			continue;
		}

		if (!IsValid(InputDataForAbility.Action))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid InputDataForAbility Action, Tag:%s"),
				__FUNCTION__, *InputDataForAbility.Tag.ToString()));
			continue;
		}

		InputForAbilityHandles.Emplace(EnhancedInputComp->BindAction(InputDataForAbility.Action, ETriggerEvent::Triggered,
			this, &ThisClass::InputForAbilityPressed, InputDataForAbility.Tag).GetHandle());
		InputForAbilityHandles.Emplace(EnhancedInputComp->BindAction(InputDataForAbility.Action, ETriggerEvent::Completed,
			this, &ThisClass::InputForAbilityReleased, InputDataForAbility.Tag).GetHandle());
	}
}

void AHeroCharacter::InputForMove(const FInputActionValue& InputActionValue)
{
	const FVector2D InputVector = InputActionValue.Get<FVector2D>();
	const FRotator MoveRotator = FRotator(0.f, GetControlRotation().Yaw, 0.f);

	if (InputVector.X != 0.f)
	{
		AddMovementInput(MoveRotator.RotateVector(FVector::RightVector), InputVector.X);
	}

	if (InputVector.Y != 0.f)
	{
		AddMovementInput(MoveRotator.RotateVector(FVector::ForwardVector), InputVector.Y);
	}
}

void AHeroCharacter::InputForLookMouseUp(const FInputActionValue& InputActionValue)
{
	const FVector2D InputVector = InputActionValue.Get<FVector2D>();

	if (InputVector.X != 0.f)
	{
		AddControllerYawInput(InputVector.X);
	}

	if (InputVector.Y != 0.f)
	{
		AddControllerPitchInput(InputVector.Y);
	}
}

void AHeroCharacter::InputForAbilityPressed(FGameplayTag InputTag)
{
	UDAbilitySystemComponent* Asc = Cast<UDAbilitySystemComponent>(GetAbilitySystemComponent());
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid DAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	Asc->AbilityForInputPressed(InputTag);
}

void AHeroCharacter::InputForAbilityReleased(FGameplayTag InputTag)
{
	UDAbilitySystemComponent* Asc = Cast<UDAbilitySystemComponent>(GetAbilitySystemComponent());
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid DAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	Asc->AbilityForInputReleased(InputTag);
}