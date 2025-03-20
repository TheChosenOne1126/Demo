// Fill out your copyright notice in the Description page of Project Settings.

#include "MessageSubsystem.h"
#include "Statics.h"

UMessageSubsystem* UMessageSubsystem::Get(const UObject* WorldContext)
{
	if (!IsValid(WorldContext))
	{
		UStatics::Log(WorldContext, ELogType::Error, FString::Printf(TEXT("%hs: invalid World Context"), __FUNCTION__));
		return nullptr;
	}

	if (!IsValid(GEngine))
	{
		UStatics::Log(WorldContext, ELogType::Error, FString::Printf(TEXT("%hs: invalid GEngine"), __FUNCTION__));
		return nullptr;
	}

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!IsValid(World))
	{
		UStatics::Log(WorldContext, ELogType::Error, FString::Printf(TEXT("%hs: invalid GEngine"), __FUNCTION__));
		return nullptr;
	}

	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!IsValid(GameInstance))
	{
		UStatics::Log(WorldContext, ELogType::Error, FString::Printf(TEXT("%hs: invalid GEngine"), __FUNCTION__));
		return nullptr;
	}

	return GameInstance->GetSubsystem<ThisClass>();
}

void UMessageSubsystem::Deinitialize()
{
	MessageLists.Reset();
	
	Super::Deinitialize();
}

FGuid UMessageSubsystem::RegisterMessageInternal(
	FGameplayTag MessageTag,
	UScriptStruct* MessageStruct,
	TFunction<void(const UScriptStruct*, const void*)>&& MessageCallback)
{
	if (!MessageTag.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MessageTag"), __FUNCTION__));
		return FGuid();
	}
	
	FMessageList* MessageList = MessageLists.FindByPredicate(
		[MessageTag](const FMessageList& List) -> bool
		{
			return List.Tag == MessageTag;
		});

	if (!MessageList)
	{
		MessageList = &MessageLists.Emplace_GetRef();
		MessageList->Tag = MessageTag;
	}
	
	FMessageItem& MessageItem = MessageList->Items.Emplace_GetRef();
	MessageItem.Callback = MessageCallback;
	MessageItem.StructType = MessageStruct;
	do
	{
		MessageItem.Id = FGuid::NewGuid();
	}
	while (MessageList->Items.ContainsByPredicate(
		[MessageItem](const FMessageItem& Item) -> bool
		{
			return Item.Id == MessageItem.Id;
		}));
	
	return MessageItem.Id;
}

void UMessageSubsystem::UnregisterMessage(FGameplayTag MessageTag, FGuid MessageId)
{
	FMessageList* MessageList = MessageLists.FindByPredicate(
		[MessageTag](const FMessageList& List) -> bool
		{
			return List.Tag == MessageTag;
		});

	if (!MessageList)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MessageList"), __FUNCTION__));
		return;
	}

	MessageList->Items.RemoveAllSwap(
		[MessageId](const FMessageItem& Item) -> bool
		{
			return Item.Id == MessageId;
		});
}

void UMessageSubsystem::BroadcastMessageInternal(FGameplayTag MessageTag, const UScriptStruct* MessageStructType, const void* Message)
{
	if (!MessageTag.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MessageTag"), __FUNCTION__));
		return;
	}
	
	FMessageList* MessageList = MessageLists.FindByPredicate(
		[MessageTag](const FMessageList& List) -> bool
		{
			return List.Tag == MessageTag;
		});

	if (!MessageList)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MessageList"), __FUNCTION__));
		return;
	}

	for	(const FMessageItem& MessageItem : MessageList->Items)
	{
		if (MessageStructType->IsChildOf(MessageItem.StructType))
		{
			MessageItem.Callback(MessageStructType, Message);
		}
	}
}
