---@type ADPlayerState
local M = UnLua.Class()
local Utils = require("Utils")
local GameplayTags = require("GameplayTags")

function M:ReceiveBeginPlay()
    local Pawn = self:GetPawn()
    if Pawn and Pawn:IsValid() then
        self:PawnSet(self, Pawn, nil)
    end

    self.OnPawnSet:Add(self, self.PawnSet)
end

function M:ReceiveEndPlay()
    self.OnPawnSet:Remove(self, self.PawnSet)

    if self:HasAuthority() then
        local Asc = UE.UAbilitySystemBlueprintLibrary.GetAbilitySystemComponent(self)
        if not Asc or not Asc:IsValid() then
            return
        end

        local Tags = UE.FGameplayTagContainer.Make_Table({
            GameplayTags.Ability_Given
        })
        UE.UAbilitySystemBlueprintLibrary.RemoveLooseGameplayTags(self, Tags)
    end
end

---@param Player APlayerState
---@param NewPawn APawn
---@param OldPawn APawn
function M:PawnSet(Player, NewPawn, OldPawn)
    if not NewPawn or not NewPawn:IsValid() or not NewPawn:IsA(UE.ADCharacter) then
        return
    end

    local bIsOk, Asc = xpcall(function(self)
        local Asc = UE.UAbilitySystemBlueprintLibrary.GetAbilitySystemComponent(self)
        if not Asc or not Asc:IsValid() then
            error("invalid Asc")
        end

        if not Asc:IsA(UE.UDAbilitySystemComponent) then
            error("Asc is not UDAbilitySystemComponent")
        end

        return Asc
    end, function(Err)
        Utils.LogError(Err)
    end, self)

    if not bIsOk then
        return
    end

    Asc:InitAbilityActorInfo(self, NewPawn)

    if not self:HasAuthority() then
        return
    end

    if Asc:HasMatchingGameplayTag(GameplayTags.Ability_Given) then
        local BaseSet = Asc:GetAttributeSet(UE.UBaseAttributeSet)
        if not BaseSet or not BaseSet:IsValid() then
            Utils.LogError("invalid UBaseAttributeSet")
            return
        end

        Asc:ApplyModToAttribute(UE.UBaseAttributeSet.GetHpAttribute(), UE.EGameplayModOp.Override, BaseSet.MaxHp.CurrentValue)
        Asc:ApplyModToAttribute(UE.UBaseAttributeSet.GetMpAttribute(), UE.EGameplayModOp.Override, BaseSet.MaxMp.CurrentValue)
    else
        local PawnData = Utils.GetPawnData(NewPawn.PawnTag)
        if not PawnData then
            Utils.LogError("nil PawnData")
            return
        end

        local AbilityDataAsset = PawnData.AbilityDataAsset
        if not AbilityDataAsset or not AbilityDataAsset:IsValid() then
            Utils.LogError("invalid AbilityDataAsset")
            return
        end

        local InitialEffectClass = AbilityDataAsset.InitialEffectClass
        if not InitialEffectClass or not InitialEffectClass:IsValid() then
            Utils.LogError("invalid InitialEffectClass")
            return
        end

        local InitialEffectSpecHandle = Asc:MakeOutgoingSpec(InitialEffectClass, 1, Asc:MakeEffectContext())
        Asc:BP_ApplyGameplayEffectSpecToSelf(InitialEffectSpecHandle)

        for AbilityClass, AbilityData in pairs(AbilityDataAsset.AbilityDataMap) do
            if not AbilityClass or not AbilityClass:IsValid() then
                Utils.LogError("invalid AbilityClass in AbilityDataMap")
                goto continue
            end

            Asc:GiveAbilityWithData(AbilityClass, AbilityData)

            ::continue::
        end

        local Tags = UE.FGameplayTagContainer.Make_Table({
            GameplayTags.Ability_Given
        })

        UE.UAbilitySystemBlueprintLibrary.AddLooseGameplayTags(self, Tags)
    end
    
end

return M