// Fill out your copyright notice in the Description page of Project Settings.

#include "DAbilityTask_SweepTrace.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Global/Statics.h"

FName UDAbilityTask_SweepTrace::SweepTraceTaskInstanceName = FName(TEXT("SweepTraceTask"));

UDAbilityTask_SweepTrace::UDAbilityTask_SweepTrace()
{
	bTickingTask = true;
}

UDAbilityTask_SweepTrace* UDAbilityTask_SweepTrace::CreateSweepTrace(UGameplayAbility* OwningAbility)
{
	ThisClass* AbilityTask = NewAbilityTask<ThisClass>(OwningAbility, SweepTraceTaskInstanceName);
	return AbilityTask;
}

void UDAbilityTask_SweepTrace::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (!bTickEnabled)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid World"), __FUNCTION__));
		bTickEnabled = false;
		return;
	}
	
	for	(FSweptInfo& SweptInfo : SweepInfos)
	{
		if (!SweptInfo.SweptComponent.IsValid())
		{
			continue;
		}

		const FVector Start = SweptInfo.LastLocation;
		const FVector End = SweptInfo.SweptComponent->GetComponentLocation();
		const FQuat Rot = SweptInfo.SweptComponent->GetComponentQuat();
		QueryParams.IgnoreMask = SweptInfo.SweptComponent->GetMoveIgnoreMask();
		QueryParams.bTraceComplex = SweptInfo.SweptComponent->bTraceComplexOnMove;
		TArray<FHitResult> SweepResults;
		if (!World->ComponentSweepMulti(SweepResults, SweptInfo.SweptComponent.Get(), Start, End, Rot, QueryParams))
		{
			continue;
		}

		if (!SweepResults.IsEmpty())
		{
			TriggerOnSwept(SweptInfo.SweptComponent.Get(), SweepResults);
		}

		SweptInfo.LastLocation = End;
	}

	SweepInfos.RemoveAllSwap(
		[](const FSweptInfo& SweptInfo) -> bool
		{
			return !SweptInfo.SweptComponent.IsValid();
		});

	if (SweepInfos.IsEmpty())
	{
		bTickEnabled = false;
	}
}

void UDAbilityTask_SweepTrace::OnDestroy(bool bInOwnerFinished)
{
	if (AbilitySystemComponent.IsValid() && Ability && Ability->GetCurrentActorInfo() && Ability->GetCurrentActorInfo()->IsNetAuthority())
	{
		AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UDAbilityTask_SweepTrace::StartSweep()
{
	UPrimitiveComponent* SweptComponent = UStatics::FindSweptComponent(GetAvatarActor());
	if (!IsValid(SweptComponent))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid NewSweptComponent"), __FUNCTION__));
		return;
	}

	SweepInfos.RemoveAllSwap(
		[SweptComponent](const FSweptInfo& SweptInfo) -> bool
		{
			return SweptInfo.SweptComponent == SweptComponent;
		});

	FSweptInfo& NewSweptInfo = SweepInfos.Emplace_GetRef();
	NewSweptInfo.SweptComponent = SweptComponent;
	NewSweptInfo.LastLocation = SweptComponent->GetComponentLocation();
	NewSweptInfo.SweepResults.Reset();

	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid World"), __FUNCTION__));
		return;
	}

	TArray<FOverlapResult> Overlaps;
	const FQuat Rotation = SweptComponent->GetComponentQuat();
	QueryParams.TraceTag = SCENE_QUERY_STAT(SweepComponent);
	QueryParams.IgnoreMask = SweptComponent->GetMoveIgnoreMask();
	if (World->ComponentOverlapMulti(Overlaps, SweptComponent, NewSweptInfo.LastLocation, Rotation, QueryParams))
	{
		TArray<FHitResult> SweepResults;
		for (const FOverlapResult& OverlapResult : Overlaps)
		{
			FHitResult& SweepResult = SweepResults.Emplace_GetRef();
			SweepResult.Component = OverlapResult.GetComponent();
			SweepResult.HitObjectHandle = OverlapResult.OverlapObjectHandle;
			SweepResult.bBlockingHit = true;
			SweepResult.Location = NewSweptInfo.LastLocation;
			SweepResult.ImpactPoint = OverlapResult.Component.IsValid() ? OverlapResult.Component->GetComponentLocation() : NewSweptInfo.LastLocation;
			SweepResult.TraceStart = NewSweptInfo.LastLocation;
			SweepResult.TraceEnd = SweepResult.ImpactPoint;
		}

		if (!SweepResults.IsEmpty())
		{
			TriggerOnSwept(SweptComponent, SweepResults);
		}
	}

	bTickEnabled = true;
}

void UDAbilityTask_SweepTrace::StopSweep()
{
	UPrimitiveComponent* SweptComponent = UStatics::FindSweptComponent(GetAvatarActor());
	if (!IsValid(SweptComponent))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid NewSweptComponent"), __FUNCTION__));
		return;
	}

	SweepInfos.RemoveAllSwap(
		[SweptComponent](const FSweptInfo& SweptInfo) -> bool
		{
			return SweptInfo.SweptComponent == SweptComponent;
		});

	if (SweepInfos.IsEmpty())
	{
		bTickEnabled = false;
	}
}

void UDAbilityTask_SweepTrace::Activate()
{
	Super::Activate();

	for (const AActor* Avatar = GetAvatarActor(); IsValid(Avatar); Avatar = Avatar->GetOwner())
	{
		QueryParams.AddIgnoredActor(Avatar);
	}

	bTickEnabled = false;

	if (!Ability || !Ability->GetCurrentActorInfo() || !Ability->GetCurrentActorInfo()->IsNetAuthority())
	{
		return;
	}

	DelegateHandle = AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).AddWeakLambda(
		this, [this](const FGameplayAbilityTargetDataHandle& Data, FGameplayTag) -> void
		{
			const FGameplayAbilityTargetDataHandle MutableData = Data;
			AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
			
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				ValidData.Broadcast(MutableData);
			}
		});
}

void UDAbilityTask_SweepTrace::TriggerOnSwept(UPrimitiveComponent* SweepComponent, TArray<FHitResult>& SweepResults)
{
	if (!IsValid(Ability))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Ability"), __FUNCTION__));
		return;
	}
	
	if (!IsValid(SweepComponent) || !SweepComponent->IsCollisionEnabled())
	{
		return;
	}

	FSweptInfo* SweptInfoPtr = SweepInfos.FindByPredicate(
		[SweepComponent](const FSweptInfo& SweptInfo) -> bool
		{
			return SweptInfo.SweptComponent == SweepComponent;
		});

	if (!SweptInfoPtr)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid SweptInfo"), __FUNCTION__));
		return;
	}

	if (!AbilitySystemComponent.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		return;
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	for (const FHitResult& SweepResult : SweepResults)
	{
		if (SweptInfoPtr->SweepResults.ContainsByPredicate(
			[SweepResult](const FHitResult& InSweepResult) -> bool
			{
				return SweepResult.GetActor() == InSweepResult.GetActor();
			}))
		{
			continue;
		}

		SweptInfoPtr->SweepResults.Emplace(SweepResult);
		TargetDataHandle.Add(new FGameplayAbilityTargetData_SingleTargetHit(SweepResult));
	}

	if (IsPredictingClient())
	{
		FScopedPredictionWindow PredictionWindow(AbilitySystemComponent.Get(), true);
		AbilitySystemComponent->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(),
			TargetDataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);
	}
	else
	{
		AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).Broadcast(TargetDataHandle, FGameplayTag());
	}
}
