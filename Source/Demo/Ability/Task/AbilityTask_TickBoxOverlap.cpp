// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityTask_TickBoxOverlap.h"
#include "Engine/OverlapResult.h"
#include "Global/Statics.h"

UAbilityTask_TickBoxOverlap::UAbilityTask_TickBoxOverlap()
{
	bTickingTask = true;
}

UAbilityTask_TickBoxOverlap* UAbilityTask_TickBoxOverlap::BoxOverlap(
	UGameplayAbility* OwningAbility,
	const FVector& BoxCenter,
	const FVector& BoxExtent,
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
	const TArray<AActor*>& ActorsToIgnore)
{
	ThisClass* AbilityTask = NewAbilityTask<ThisClass>(OwningAbility);
	AbilityTask->BoxCenter = BoxCenter;
	AbilityTask->BoxExtent = BoxExtent;
	AbilityTask->ObjectTypes = ObjectTypes;
	AbilityTask->ActorsToIgnore = ActorsToIgnore;
	return AbilityTask;	
}

void UAbilityTask_TickBoxOverlap::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs invalid UWorld"), __FUNCTION__));
		return;
	}
	
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AbiltiyTask_TickBoxOverlap), false);
	Params.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;
	FCollisionObjectQueryParams ObjectParams;
	const ECollisionChannel& Channel = UCollisionProfile::Get()->ConvertToCollisionChannel(false, ObjectType);
	ObjectParams.AddObjectTypesToQuery(Channel);

	const bool bIsOverlap = World->OverlapMultiByObjectType(
		Overlaps,
		BoxCenter,
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeBox(BoxExtent),
		Params);

	TArray<AActor*> OverlapActors;
	for (FOverlapResult& Result : Overlaps)
	{
		OverlapActors.Emplace(Result.GetActor());
	}
	
	Overlap.Broadcast(bIsOverlap, OverlapActors);
}

void UAbilityTask_TickBoxOverlap::UpdateBoxCenter(const FVector& InBoxCenter)
{
	BoxCenter = InBoxCenter;
}