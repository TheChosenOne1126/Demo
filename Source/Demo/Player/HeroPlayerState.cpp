// Fill out your copyright notice in the Description page of Project Settings.

#include "HeroPlayerState.h"
#include "Attribute/HeroAttributeSet.h"

AHeroPlayerState::AHeroPlayerState()
{
	CreateDefaultSubobject<UHeroAttributeSet>(TEXT("HeroAttributeSet"));
}