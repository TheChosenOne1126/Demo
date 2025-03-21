// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

namespace GlobalTags
{
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputMoveTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputLookMouseTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivationInputPressedTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivationInputHeldTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivationAutoTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityConfirmTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityCancelTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityUpdateLevelTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityOnGivenTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilitySystemInitTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityUltimateTag)
	//DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityUltimateTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EventDeadTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EventMovementWalkTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EventOnHitTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateBlockHpRegenTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateBlockMpRegenTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HeroStrengthTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HeroIntelligenceTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HeroAgilityTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCallerAbilityMaxLevelTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCallerCooldownValueTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCallerPerHpCostTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCallerPerMaxHpCostTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCallerPerMpCostTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCallerPerMaxMpCostTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCallerFixedDamageTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCallerPerHpDamageTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCallerPerMaxHpDamageTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCallerPerDamageAttributeTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCallerPerExtraDamageAttributeTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AttributeBaseAbilityPointChangedTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AttributeUltimateAbilityPointChangedTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EffectDamageNormalAttackTag)
	DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EffectDamageMissTag)
}