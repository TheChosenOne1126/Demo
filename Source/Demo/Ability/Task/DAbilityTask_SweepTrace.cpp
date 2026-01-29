// Fill out your copyright notice in the Description page of Project Settings.

#include "DAbilityTask_SweepTrace.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Global/Statics.h"

UDAbilityTask_SweepTrace::UDAbilityTask_SweepTrace()
{
	bTickingTask = true;
}

UDAbilityTask_SweepTrace* UDAbilityTask_SweepTrace::CreateSweepTrace(UGameplayAbility* OwningAbility)
{
	ThisClass* AbilityTask = NewAbilityTask<ThisClass>(OwningAbility);
	return AbilityTask;
}

void UDAbilityTask_SweepTrace::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (!bCanTrace)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid World"), __FUNCTION__));
		return;
	}
	
	const FVector Start = LastLocation;
	const FVector End = SweptComponent->GetComponentLocation();
	const FQuat Rot = SweptComponent->GetComponentQuat();
	TArray<FHitResult> HitResults;
	if (!World->ComponentSweepMulti(HitResults, SweptComponent.Get(), Start, End, Rot, QueryParams))
	{
		return;
	}

	if (!HitResults.IsEmpty())
	{
		TriggerOnSwept(HitResults);
	}

	LastLocation = End;

	if (!SweptComponent.IsValid())
	{
		EndTask();
	}
}

void UDAbilityTask_SweepTrace::OnDestroy(bool bInOwnerFinished)
{
	if (AbilitySystemComponent.IsValid() && Ability && Ability->GetCurrentActorInfo() && Ability->GetCurrentActorInfo()->IsNetAuthority())
	{
		AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
	}

	SweepResults.Reset();

	Super::OnDestroy(bInOwnerFinished);
}

void UDAbilityTask_SweepTrace::StartLogic(bool bInitialOverlap)
{
	if (!IsLocallyControlled())
	{
		return;
	}
	
	if (!SweptComponent.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid NewSweptComponent"), __FUNCTION__));
		return;
	}

	SweepResults.Reset();

	LastLocation = SweptComponent->GetComponentLocation();

	bCanTrace = true;

	if (!bInitialOverlap)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid World"), __FUNCTION__));
		return;
	}

	TArray<FOverlapResult> Overlaps;
	const FQuat Rotation = SweptComponent->GetComponentQuat();
	if (World->ComponentOverlapMulti(Overlaps, SweptComponent.Get(), LastLocation, Rotation, QueryParams))
	{
		TArray<FHitResult> HitResults;
		for (const FOverlapResult& OverlapResult : Overlaps)
		{
			FHitResult& SweepResult = HitResults.Emplace_GetRef();
			SweepResult.Component = OverlapResult.GetComponent();
			SweepResult.HitObjectHandle = OverlapResult.OverlapObjectHandle;
			SweepResult.bBlockingHit = true;
			SweepResult.Location = LastLocation;
			SweepResult.ImpactPoint = OverlapResult.Component.IsValid() ? OverlapResult.Component->GetComponentLocation() : LastLocation;
			SweepResult.TraceStart = LastLocation;
			SweepResult.TraceEnd = SweepResult.ImpactPoint;
		}

		if (!HitResults.IsEmpty())
		{
			TriggerOnSwept(HitResults);
		}
	}
}

void UDAbilityTask_SweepTrace::StopLogic()
{
	bCanTrace = false;
}

void UDAbilityTask_SweepTrace::Activate()
{
	Super::Activate();

	if (IsLocallyControlled())
	{
		for (const AActor* Avatar = GetAvatarActor(); IsValid(Avatar); Avatar = Avatar->GetOwner())
		{
			QueryParams.AddIgnoredActor(Avatar);
		}

		SweptComponent = UStatics::FindSweptComponent(GetAvatarActor());

		QueryParams.IgnoreMask = SweptComponent->GetMoveIgnoreMask();
		QueryParams.bTraceComplex = SweptComponent->bTraceComplexOnMove;

		bCanTrace = false;
	}

	DelegateHandle = AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).AddWeakLambda(
		this, [this](const FGameplayAbilityTargetDataHandle& Data, FGameplayTag) -> void
		{
			if (IsPredictingClient())
			{
				FScopedPredictionWindow PredictionWindow(AbilitySystemComponent.Get(), true);
				AbilitySystemComponent->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(),
					Data, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);
			}
			else
			{
				AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
			}
			
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				ValidData.Broadcast(Data);
			}
		});
}

void UDAbilityTask_SweepTrace::TriggerOnSwept(TArray<FHitResult>& HitResults) const
{
	if (!IsValid(Ability))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
		return;
	}
	
	if (!SweptComponent.IsValid() || !SweptComponent->IsCollisionEnabled())
	{
		return;
	}

	if (!AbilitySystemComponent.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		return;
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	for (const FHitResult& HitResult : HitResults)
	{
		if (HitResults.ContainsByPredicate(
			[HitResult](const FHitResult& InSweepResult) -> bool
			{
				return HitResult.GetActor() == InSweepResult.GetActor();
			}))
		{
			continue;
		}

		HitResults.Emplace(HitResult);
		TargetDataHandle.Add(new FGameplayAbilityTargetData_SingleTargetHit(HitResult));
	}

	AbilitySystemComponent->ConfirmAbilityTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), TargetDataHandle, FGameplayTag());
}
