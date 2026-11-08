// Fill out your copyright notice in the Description page of Project Settings.

#include "GlobalTags.h"

namespace GlobalTags
{
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(IATAG_MOVE, "InputTag.Move", "Input Tag For Move(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(IATAG_LOOKMOUSE, "InputTag.Look(Mouse)", "Input Tag For Look Mouse(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(IMC, "InputMappingContext", "Input Mapping Context Root Tag(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(PAWN, "Pawn", "Pawn Root Tag(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(CM, "CameraModifier", "Camera Modifier Root Tag(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GA_INPUT_PRESS, "GA.Input.Pressed", "Ability Activation Input Pressed(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GA_INPUT_HELD, "GA.Input.Held", "Ability Activation Input Held(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GA_PASSIVE, "GA.Passive", "Ability Activation Auto(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GA_UPDATE_LV, "GA.UpdateLevel", "Ability Update Level(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GA_SLOT_INIT, "GA.Slot.Init", "Ability Slot Initialized(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GA_GIVEN, "GA.Given", "All initial Abilities Given (Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GA_SLOT, "GA.Slot", "Ability Slot Root Tag(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GA_ULTIMATE, "GA.Ultimate", "Ability type is ultimate(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GA_INPUT_START, "GA.Input.Start", "Ability input action start(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GA_INPUT_TRIGGER, "GA.Input.Trigger", "Ability input action trigger(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GA_INPUT_CANCEL, "GA.Input.Cancel", "Ability input action cancel(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(SETBYCALLER_GA_MAXLV, "SetByCaller.GA.MaxLv", "Set Max Ability Level when Ability is Given(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(SETBYCALLER_GA_COOLDOWN, "SetByCaller.GA.Cooldown", "Set the Cooldown Value when Ability ApplyCooldown(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GE_ATTACK_NORMAL, "GE.Attack.Normal", "Pawn is damaged by normally attack(Native)")
	DEMO_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(GE_ATTACK_MISS, "GE.Attack.Miss", "Pawn is missed by one damage")
}