// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "DAnimNotifyState_GenericEventByTag.generated.h"

UCLASS()
class DEMO_API UDAnimNotifyState_GenericEventByTag : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UDAnimNotifyState_GenericEventByTag();
	
	virtual FString GetNotifyName_Implementation() const override;
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Event")
	FString NotifyName;
	
	UPROPERTY(EditInstanceOnly, Category = "Event")
	FGameplayTag StartEventTag;
	
	UPROPERTY(EditInstanceOnly, Category = "Event")
	FGameplayTag EndEventTag;
};
