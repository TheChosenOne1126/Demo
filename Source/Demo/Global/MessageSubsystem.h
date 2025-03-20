// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MessageSubsystem.generated.h"

USTRUCT()
struct FMessageItem
{
	GENERATED_BODY()

	TFunction<void(const UScriptStruct*, const void*)> Callback = nullptr;

	FGuid Id;

	UPROPERTY()
	TObjectPtr<UScriptStruct> StructType;
};

USTRUCT()
struct FMessageList
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	TArray<FMessageItem> Items;
};

UCLASS()
class DEMO_API UMessageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UMessageSubsystem* Get(const UObject* WorldContext);

	virtual void Deinitialize() override;
	
	template<typename TMessageStructType, typename TOwner = UObject>
	FGuid RegisterMessage(FGameplayTag MessageTag, TOwner* Owner, void(TOwner::* MessageCallback)(const TMessageStructType&));

	void UnregisterMessage(FGameplayTag MessageTag, FGuid MessageId);

	template<typename TMessageStructType>
	void BroadcastMessage(FGameplayTag InMessageTag, const TMessageStructType& MessageStruct);

protected:
	void BroadcastMessageInternal(FGameplayTag MessageTag, const UScriptStruct* MessageStructType, const void* Message);
	
	FGuid RegisterMessageInternal(
		FGameplayTag MessageTag,
		UScriptStruct* MessageStruct,
		TFunction<void(const UScriptStruct*, const void*)>&& MessageCallback);
	
private:
	UPROPERTY()
	TArray<FMessageList> MessageLists;
};

template <typename TMessageStruct, typename TOwner>
FGuid UMessageSubsystem::RegisterMessage(FGameplayTag MessageTag, TOwner* Owner, void(TOwner::* MessageCallback)(const TMessageStruct&))
{
	UScriptStruct* MessageStruct = TBaseStructure<TMessageStruct>::Get();
	return RegisterMessageInternal(MessageTag, MessageStruct,
		[Owner, MessageCallback](const UScriptStruct*, const void* Message) -> void
		{
			if (IsValid(Owner))
			{
				(Owner->*MessageCallback)(*static_cast<const TMessageStruct*>(Message));
			}
		});
}

template <typename TMessageStruct>
void UMessageSubsystem::BroadcastMessage(FGameplayTag InMessageTag, const TMessageStruct& MessageStruct)
{
	const UScriptStruct* MessageStructType = TBaseStructure<TMessageStruct>::Get();
	BroadcastMessageInternal(InMessageTag, MessageStructType, &MessageStruct);
}
