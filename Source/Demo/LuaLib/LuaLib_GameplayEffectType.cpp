#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "UnLuaEx.h"

static int32 FGameplayEffectSpecHandle_AssignTagSetByCallerMagnitude(lua_State* L)
{
	const int32 NumParams = lua_gettop(L);
	if (NumParams != 3)
	{
		return luaL_error(L, "invalid parameters");
	}

	const FGameplayEffectSpecHandle* Handle = static_cast<FGameplayEffectSpecHandle*>(GetCppInstanceFast(L, 1));
	if (!Handle || !Handle->IsValid())
	{
		return luaL_error(L, "invalid FGameplayEffectSpecHandle");
	}

	const FGameplayTag* Tag = static_cast<FGameplayTag*>(GetCppInstanceFast(L, 2));
	if (!Tag || !Tag->IsValid())
	{
		return luaL_error(L, "invalid FGameplayTag");
	}
	
	const float Magnitude = static_cast<float>(lua_tonumber(L, 3));
	Handle->Data->SetSetByCallerMagnitude(*Tag, Magnitude);

	return 1;
}

static constexpr luaL_Reg FGameplayEffectSpecHandleLib[] =
{
	{"AssignTagSetByCallerMagnitude", FGameplayEffectSpecHandle_AssignTagSetByCallerMagnitude},
	{nullptr, nullptr}
};

BEGIN_EXPORT_REFLECTED_CLASS(FGameplayEffectSpecHandle)
	ADD_LIB(FGameplayEffectSpecHandleLib)
END_EXPORT_CLASS()

IMPLEMENT_EXPORTED_CLASS(FGameplayEffectSpecHandle)