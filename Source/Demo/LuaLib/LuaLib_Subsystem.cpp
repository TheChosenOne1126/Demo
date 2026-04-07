#include "UnLuaEx.h"
#include "UnLuaBase.h"
#include "UI/GlobalUISubsystem.h"
using namespace UnLua;

static int32 UGlobalUISubsystem_Get(lua_State* L)
{
	const int32 NumParams = lua_gettop(L);
	if (NumParams != 1)
	{
		return luaL_error(L, "invalid parameters");
	}

	const UObject* ContextObject = GetUObject(L, 1);
	if (!ContextObject)
	{
		return luaL_error(L, "invalid context object");
	}

	const UWorld* World = ContextObject->GetWorld();
	if (!IsValid(World))
	{
		return luaL_error(L, "invalid world");
	}

	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!IsValid(GameInstance))
	{
		return luaL_error(L, "invalid game instance");
	}

	if (GameInstance->IsDedicatedServerInstance())
	{
		return luaL_error(L, "No run Dedicated server instance");
	}

	const ULocalPlayer* LocalPlayer = GameInstance->GetFirstGamePlayer();
	if (!IsValid(LocalPlayer))
	{
		return luaL_error(L, "invalid local player");
	}
	
	UGlobalUISubsystem* GlobalUISubsystem = LocalPlayer->GetSubsystem<UGlobalUISubsystem>();
	PushUObject(L, GlobalUISubsystem);
	
	return 1;
}

static constexpr luaL_Reg UGlobalUISubsystemLib[] = {
	{ "Get", UGlobalUISubsystem_Get },
	{ nullptr, nullptr }
};

BEGIN_EXPORT_REFLECTED_CLASS(UGlobalUISubsystem)
	ADD_LIB(UGlobalUISubsystemLib)
END_EXPORT_CLASS()