// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DCharacter.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "HeroCharacter.generated.h"

UCLASS(Abstract)
class DEMO_API AHeroCharacter : public ADCharacter
{
	GENERATED_BODY()

public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void DestroyPlayerInputComponent() override;

	void BindInputActions(UInputComponent* PlayerInputComponent, const UInputDataAsset* InputDataAsset);

	void InputForMove(const FInputActionValue& InputActionValue);

	void InputForLookMouseUp(const FInputActionValue& InputActionValue);

	void InputForAbilityPressed(FGameplayTag InputTag);

	void InputForAbilityReleased(FGameplayTag InputTag);
	
private:
	UPROPERTY(Transient)
	TOptional<uint32> InputForMoveHandle;

	UPROPERTY(Transient)
	TOptional<uint32> InputForLookMouseHandle;

	UPROPERTY(Transient)
	TArray<uint32> InputForAbilityHandles;
};
