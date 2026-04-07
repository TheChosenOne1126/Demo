#include "UnLuaEx.h"
#include "UnLuaBase.h"
#include "LuaEnv.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
using namespace UnLua;

/**
 * BindAction 的 Lua 封装
 * 
 * 核心问题：
 *   C++ 模板版本: BindAction(IA, Event, this, &Func, InputTag)
 *     匹配 FEnhancedInputActionHandlerSignature (void())，InputTag 作为绑定参数
 *   FName 版本: BindAction(IA, Event, Object, FName)
 *     匹配 FEnhancedInputActionHandlerDynamicSignature (void(FInputActionValue, float, float, const UInputAction*))
 *   两者签名完全不同，不能互通。
 * 
 * 解决方案：
 *   使用 BindActionValueLambda 创建 lambda 绑定，
 *   在 lambda 中拿到 lua_State，手动调用 Lua 函数并传递所有参数。
 * 
 * 用法：
 *   -- 不带额外参数: function(self, ActionValue)
 *   EIC:BindAction(IA, ETriggerEvent.Triggered, self, "OnMove")
 *
 *   -- 带 FGameplayTag 额外参数: function(self, InputTag, ActionValue)
 *   EIC:BindAction(IA, ETriggerEvent.Triggered, self, "OnAbilityPressed", InputTag)
 *
 *   -- 直接传入 Lua function: function(self, InputTag, ActionValue)
 *   EIC:BindAction(IA, ETriggerEvent.Triggered, self, function(self, InputTag, ActionValue) end, InputTag)
 *
 * 参数传递顺序: self, [额外参数...], ActionValue
 */
static int32 UEnhancedInputComponent_BindAction(lua_State* L)
{
	const int32 NumParams = lua_gettop(L);
	if (NumParams < 4)
	{
		return luaL_error(L, "invalid parameters, at least 4 required");
	}

	// 获取 FLuaEnv，后续 lambda 中通过 Env->GetMainState() 获取 lua_State*
	FLuaEnv* Env = FLuaEnv::FindEnv(L);
	if (!Env)
	{
		return luaL_error(L, "FLuaEnv not found");
	}

	// 参数1: self (UEnhancedInputComponent*)
	UEnhancedInputComponent* Eic = Cast<UEnhancedInputComponent>(GetUObject(L, 1));
	if (!Eic)
	{
		return luaL_error(L, "invalid UEnhancedInputComponent");
	}

	// 参数2: UInputAction*
	UInputAction* IA = Cast<UInputAction>(GetUObject(L, 2));
	if (!IA)
	{
		return luaL_error(L, "invalid UInputAction");
	}

	// 参数3: ETriggerEvent
	const ETriggerEvent Event = static_cast<ETriggerEvent>(static_cast<int32>(lua_tointeger(L, 3)));

	// 参数4: UObject* (self 对象)，使用 TWeakObjectPtr 安全引用
	UObject* Target = GetUObject(L, 4);
	if (!Target)
	{
		return luaL_error(L, "invalid target object");
	}
	TWeakObjectPtr WeakTarget(Target);

	// 参数5: 函数名(string) 或 Lua function
	int32 FuncRef;

	if (lua_isfunction(L, 5))
	{
		// 直接传入 Lua function
		// 约定：function 的第一个参数必须是 self，与 string 方式一致
		// 正确用法: function(self, InputTag) ... end
		// 错误用法: function(InputTag) ... end  ← self 会被当作 InputTag
		lua_pushvalue(L, 5);
		FuncRef = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	else if (lua_isstring(L, 5))
	{
		// 通过函数名从对象的 Lua table 中查找函数
		const char* FuncName = lua_tostring(L, 5);

		// 使用 UnLua::PushUObject 将 Target 对应的 Lua table push 到栈上
		PushUObject(L, Target);
		if (!lua_istable(L, -1))
		{
			lua_pop(L, 1);
			return luaL_error(L, "target object is not bound to lua");
		}

		lua_getfield(L, -1, FuncName);
		if (!lua_isfunction(L, -1))
		{
			lua_pop(L, 2);
			return luaL_error(L, "function '%s' not found in lua module", FuncName);
		}
		FuncRef = luaL_ref(L, LUA_REGISTRYINDEX);   // ref function, pop function
		lua_pop(L, 1);                               // pop self table
	}
	else
	{
		return luaL_error(L, "5th parameter must be string or function");
	}

	// 参数6+: 收集可变参数的 LuaRef
	TArray<int32> ArgRefs;
	for (int32 i = 6; i <= NumParams; ++i)
	{
		lua_pushvalue(L, i);
		ArgRefs.Add(luaL_ref(L, LUA_REGISTRYINDEX));
	}

	// 使用 BindActionValueLambda：在 lambda 中通过 FLuaEnv 获取 lua_State，
	// 通过 TWeakObjectPtr + PushUObject 推送 self，通过 UnLua::Push 推送 ActionValue
	FEnhancedInputActionEventBinding& Binding = Eic->BindActionValueLambda(
		IA,
		Event,
		[Env, FuncRef, WeakTarget, ArgRefs](const FInputActionValue& ActionValue)
		{
			if (FuncRef == LUA_NOREF)
			{
				return;
			}

			// 通过 FLuaEnv 获取 lua_State*
			lua_State* LL = Env->GetMainState();
			if (!LL)
			{
				return;
			}

			// 通过 TWeakObjectPtr 检查 Target 是否仍然有效
			if (!WeakTarget.IsValid())
			{
				return;
			}

			// push 错误处理函数
			lua_pushcfunction(LL, ReportLuaCallError);
			const int32 ErrFuncIdx = lua_gettop(LL);

			// push Lua function
			lua_rawgeti(LL, LUA_REGISTRYINDEX, FuncRef);

			// push self：通过 UnLua::PushUObject 将 Target 的 Lua table 推送到栈上
			PushUObject(LL, WeakTarget.Get());

			// push 额外参数
			for (int32 ArgRef : ArgRefs)
			{
				lua_rawgeti(LL, LUA_REGISTRYINDEX, ArgRef);
			}

			// push ActionValue：通过 UnLua::Push (bCopy=true) 创建 Lua 端的拷贝
			Push(LL, ActionValue, true);

			// 调用：self + N个额外参数 + ActionValue
			const int32 NumArgs = 1 + ArgRefs.Num() + 1;
			lua_pcall(LL, NumArgs, 0, ErrFuncIdx);

			lua_pop(LL, 1); // pop error handler
		}
	);

	lua_pushinteger(L, Binding.GetHandle());
	return 1;
}

/**
 * RemoveBindingByHandle 的 Lua 封装
 */
static int32 UEnhancedInputComponent_RemoveBindingByHandle(lua_State* L)
{
	const int32 NumParams = lua_gettop(L);
	if (NumParams < 2)
	{
		return luaL_error(L, "invalid parameters");
	}

	UEnhancedInputComponent* Eic = Cast<UEnhancedInputComponent>(GetUObject(L, 1));
	if (!Eic)
	{
		return luaL_error(L, "invalid UEnhancedInputComponent");
	}

	const uint32 Handle = static_cast<uint32>(lua_tointeger(L, 2));
	lua_pushboolean(L, Eic->RemoveBindingByHandle(Handle));
	return 1;
}

static constexpr luaL_Reg UEnhancedInputComponentLib[] = {
	{ "BindAction", UEnhancedInputComponent_BindAction },
	{ "RemoveBindingByHandle", UEnhancedInputComponent_RemoveBindingByHandle },
	{ nullptr, nullptr }
};

BEGIN_EXPORT_REFLECTED_CLASS(UEnhancedInputComponent)
	ADD_LIB(UEnhancedInputComponentLib)
END_EXPORT_CLASS()

IMPLEMENT_EXPORTED_CLASS(UEnhancedInputComponent)

