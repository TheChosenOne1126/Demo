#include "UnLuaEx.h"
#include "Attribute/BaseAttributeSet.h"
#include "Attribute/HeroAttributeSet.h"

BEGIN_EXPORT_REFLECTED_CLASS(UBaseAttributeSet)
	ADD_STATIC_FUNCTION(GetHpAttribute)
	ADD_STATIC_FUNCTION(GetMaxHpAttribute)
	ADD_STATIC_FUNCTION(GetHpRegenAttribute)
	ADD_STATIC_FUNCTION(GetMpAttribute)
	ADD_STATIC_FUNCTION(GetMaxMpAttribute)
	ADD_STATIC_FUNCTION(GetMpRegenAttribute)
	ADD_STATIC_FUNCTION(GetLvAttribute)
	ADD_STATIC_FUNCTION(GetMaxLvAttribute)
	ADD_STATIC_FUNCTION(GetDamageAttribute)
	ADD_STATIC_FUNCTION(GetArmorAttribute)
	ADD_STATIC_FUNCTION(GetAttackSpeedAttribute)
END_EXPORT_CLASS()

IMPLEMENT_EXPORTED_CLASS(UBaseAttributeSet)


BEGIN_EXPORT_REFLECTED_CLASS(UHeroAttributeSet)
	ADD_STATIC_FUNCTION(GetXpAttribute)
	ADD_STATIC_FUNCTION(GetBaseSpAttribute)
	ADD_STATIC_FUNCTION(GetUltimateSpAttribute)
	ADD_STATIC_FUNCTION(GetMaxSpAttribute)
END_EXPORT_CLASS()

IMPLEMENT_EXPORTED_CLASS(UHeroAttributeSet)