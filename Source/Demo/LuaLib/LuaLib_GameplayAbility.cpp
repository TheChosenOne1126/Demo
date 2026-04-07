#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "UnLuaEx.h"
#include "UnLuaBase.h"
using namespace UnLua;

BEGIN_EXPORT_REFLECTED_CLASS(UGameplayAbility)
	ADD_FUNCTION(IsPredictingClient)
	ADD_FUNCTION(IsForRemoteClient)
END_EXPORT_CLASS()

IMPLEMENT_EXPORTED_CLASS(UGameplayAbility)

static int32 UAbilitySystemComponent_ApplyModToAttribute(lua_State* L)
{
	const int32 NumParams = lua_gettop(L);
	if (NumParams != 3)
	{
		return luaL_error(L, "invalid parameters");
	}
	
	UAbilitySystemComponent* Asc = Cast<UAbilitySystemComponent>(GetUObject(L, 1));
	if (!IsValid(Asc))
	{
		return luaL_error(L, "invalid UAbilitySystemComponent");
	}
	
	FGameplayAttribute* Attribute = static_cast<FGameplayAttribute*>(GetCppInstanceFast(L, 2));
	if (!Attribute)
	{
		return luaL_error(L, "invalid FGameplayAttribute");
	}

	const int ModifierOp = lua_tointeger(L, 3);
	const float ModifierMagnitude = lua_tonumber(L, 4);
	
	if (IsValid(Asc))
	{
		Asc->ApplyModToAttribute(*Attribute, static_cast<TEnumAsByte<EGameplayModOp::Type>>(ModifierOp), ModifierMagnitude);
	}
	
	return 1;
}

static int32 UAbilitySystemComponent_FindAbilitySpecFromHandle(lua_State* L)
{
	const int32 NumParams = lua_gettop(L);
	if (NumParams < 2 || NumParams > 3)
	{
		return luaL_error(L, "invalid parameters");
	}

	const UAbilitySystemComponent* Asc = Cast<UAbilitySystemComponent>(GetUObject(L, 1));
	if (!IsValid(Asc))
	{
		return luaL_error(L, "invalid UAbilitySystemComponent");
	}

	const FGameplayAbilitySpecHandle* Handle = static_cast<FGameplayAbilitySpecHandle*>(GetCppInstanceFast(L, 2));
	if (!Handle)
	{
		return luaL_error(L, "invalid FGameplayAbilitySpecHandle");
	}

	EConsiderPending ConsiderPending = EConsiderPending::PendingRemove;
	if (NumParams == 3)
	{
		ConsiderPending = static_cast<EConsiderPending>(static_cast<int32>(lua_tointeger(L, 3)));
	}
	
	FGameplayAbilitySpec* Spec = Asc->FindAbilitySpecFromHandle(*Handle, ConsiderPending);
	if (!Spec)
	{
		return luaL_error(L, "invalid FGameplayAbilitySpec");
	}
	
	Push(L, Spec);

	return 1;
}

static constexpr luaL_Reg UAbilitySystemComponentLib[] = {
	{ "FindAbilitySpecFromHandle", UAbilitySystemComponent_FindAbilitySpecFromHandle },
	{ "ApplyModToAttribute", UAbilitySystemComponent_ApplyModToAttribute },
	{ nullptr, nullptr }
};

BEGIN_EXPORT_REFLECTED_CLASS(UAbilitySystemComponent)
	ADD_FUNCTION(IncrementAbilityListLock)
	ADD_FUNCTION(DecrementAbilityListLock)
	ADD_FUNCTION(HandleGameplayEvent)
	ADD_FUNCTION(InitAbilityActorInfo)
	ADD_FUNCTION(AbilitySpecInputPressed)
	ADD_FUNCTION(AbilitySpecInputReleased)
	ADD_LIB(UAbilitySystemComponentLib)
END_EXPORT_CLASS()

IMPLEMENT_EXPORTED_CLASS(UAbilitySystemComponent)


BEGIN_EXPORT_ENUM(EConsiderPending)
	ADD_SCOPED_ENUM_VALUE(None)
	ADD_SCOPED_ENUM_VALUE(PendingAdd)
	ADD_SCOPED_ENUM_VALUE(PendingRemove)
	ADD_SCOPED_ENUM_VALUE(All)
END_EXPORT_ENUM(EConsiderPending)


BEGIN_EXPORT_REFLECTED_CLASS(FGameplayAbilitySpec)
	ADD_FUNCTION(IsActive)
	ADD_FUNCTION(GetPrimaryInstance)
END_EXPORT_CLASS()

IMPLEMENT_EXPORTED_CLASS(FGameplayAbilitySpec)