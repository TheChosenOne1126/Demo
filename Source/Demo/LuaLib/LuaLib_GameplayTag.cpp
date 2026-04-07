#include "UnLuaEx.h"
#include "LuaCore.h"
#include "GameplayTagContainer.h"
using namespace UnLua;

static int32 FGameplayTag_RequestGameplayTag(lua_State* L)
{
	const int32 NumParams = lua_gettop(L);
	if (NumParams < 1)
	{
		return luaL_error(L, "invalid parameters");
	}

	const char* TagName = lua_tostring(L, 1);
	if (!TagName)
	{
		return luaL_error(L, "invalid tag name");
	}

	bool ErrorIfNotFound = true;
	if (NumParams >= 2)
	{
		ErrorIfNotFound = static_cast<bool>(lua_toboolean(L, 2));
	}

	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TagName), ErrorIfNotFound);
	if (!Tag.IsValid())
	{
		return luaL_error(L, "invalid tag");
	}

	Push(L, Tag, true);

	return 1;
}

static constexpr luaL_Reg FGameplayTagLib[] = {
	{ "RequestGameplayTag", FGameplayTag_RequestGameplayTag },
	{ nullptr, nullptr }
};

BEGIN_EXPORT_REFLECTED_CLASS(FGameplayTag)
	ADD_FUNCTION(MatchesTagExact)
	ADD_FUNCTION(MatchesTag)
	ADD_FUNCTION(IsValid)
	ADD_LIB(FGameplayTagLib)
END_EXPORT_CLASS()

IMPLEMENT_EXPORTED_CLASS(FGameplayTag)

static int32 FGameplayTagContainer_MakeByTable(lua_State* L)
{
	const int32 NumParams = lua_gettop(L);
	if (NumParams != 1)
	{
		return luaL_error(L, "invalid parameters");
	}

	if (!lua_istable(L, 1))
	{
		return luaL_error(L, "expected a table of FGameplayTag");
	}

	void* Userdata = NewTypedUserdata(L, FGameplayTagContainer);
	if (Userdata)
	{
		FGameplayTagContainer* NewContainer = new(Userdata) FGameplayTagContainer();
		const int32 Len = lua_rawlen(L, 1);
		for (int32 i = 1; i <= Len; ++i)
		{  
			lua_rawgeti(L, 1, i);
			const FGameplayTag* Tag = static_cast<FGameplayTag*>(GetCppInstanceFast(L, -1));
			if (Tag && Tag->IsValid())
			{
				NewContainer->AddTag(*Tag);
			}
			lua_pop(L, 1);
		}
	}

	return 1;
}

static constexpr luaL_Reg FGameplayTagContainerLib[] = {
	{"Make_Table", FGameplayTagContainer_MakeByTable},
	{nullptr, nullptr}
};

BEGIN_EXPORT_REFLECTED_CLASS(FGameplayTagContainer)
	ADD_FUNCTION(HasTag)
	ADD_FUNCTION(HasTagExact)
	ADD_FUNCTION(RemoveTags)
	ADD_FUNCTION(AppendTags)
	ADD_FUNCTION(AddTag)
	ADD_FUNCTION(Reset)
	ADD_FUNCTION(IsValid)
	ADD_LIB(FGameplayTagContainerLib)
END_EXPORT_CLASS()

IMPLEMENT_EXPORTED_CLASS(FGameplayTagContainer)