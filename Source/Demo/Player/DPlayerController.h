// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "UnLuaInterface.h"
#include "GameFramework/PlayerController.h"
#include "Interface/PawnTagInterface.h"
#include "DPlayerController.generated.h"

UCLASS()
class DEMO_API ADPlayerController : public APlayerController, public IUnLuaInterface, public IPawnTagInterface
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
	
	virtual FGameplayTag GetPawnTag() const override;
	
	virtual void SetPawnTag(const FGameplayTag& NewPawnTag) override;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetPlayerTag(const FGameplayTag& NewPlayerTag);
	
protected:
	void OnNativeInput(const FInputActionValue& Value, const FGameplayTag& Tag);
	
	void OnMoveInput(const FInputActionValue& Value) const;
	
	void OnLookInput(const FInputActionValue& Value);
	
	void OnAbilityTrigger(const FGameplayTag& Tag) const;
	
	void OnAbilityStart(const FGameplayTag& Tag) const;
	
	void OnAbilityCancel(const FGameplayTag& Tag) const;
	
	UFUNCTION()
	void OnRep_PlayerTag();
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerTag)
	FGameplayTag PlayerTag;
};
