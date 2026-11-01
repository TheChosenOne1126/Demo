// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "UnLuaInterface.h"
#include "GameFramework/PlayerController.h"
#include "DPlayerController.generated.h"

class UDAbilitySystemComponent;
class UHUDWidget;

UCLASS()
class DEMO_API ADPlayerController : public APlayerController, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
	virtual FString GetModuleName_Implementation() const override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Client, Reliable)
	void ClientPostLogin();
	
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnClientPostLogin();
	
	UFUNCTION()
	void OnRep_PlayerTag();
	
	void SetPlayerTag(const FGameplayTag& NewTag);
	
protected:
	void OnNativeInput(const FInputActionValue& Value, const FGameplayTag& Tag);
	
	void OnMoveInput(const FInputActionValue& Value);
	
	void OnLookInput(const FInputActionValue& Value);
	
	void OnAbilityTrigger(const FGameplayTag& Tag);
	
	void OnAbilityStart(const FGameplayTag& Tag);
	
	void OnAbilityCancel(const FGameplayTag& Tag);
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerTag)
	FGameplayTag PlayerTag;
};
