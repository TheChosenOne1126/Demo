// Fill out your copyright notice in the Description page of Project Settings.

#include "DAnimNotifyState_GenericEventByTag.h"
#include "Component/DAbilitySystemComponent.h"
#include "Global/Statics.h"

UDAnimNotifyState_GenericEventByTag::UDAnimNotifyState_GenericEventByTag()
{
	bShouldFireInEditor = false;
}

FString UDAnimNotifyState_GenericEventByTag::GetNotifyName_Implementation() const
{
	return NotifyName;
}

void UDAnimNotifyState_GenericEventByTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!IsValid(MeshComp))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MeshComp"), __FUNCTION__));
		return;
	}
	
	UDAbilitySystemComponent* Asc = UStatics::GetDAbilitySystemComponent(MeshComp->GetOwner());
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid DAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	Asc->HandleGameplayEvent(StartEventTag, nullptr);
}

void UDAnimNotifyState_GenericEventByTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MeshComp"), __FUNCTION__));
		return;
	}

	UDAbilitySystemComponent* Asc = UStatics::GetDAbilitySystemComponent(MeshComp->GetOwner());
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid DAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	Asc->HandleGameplayEvent(EndEventTag, nullptr);
}
