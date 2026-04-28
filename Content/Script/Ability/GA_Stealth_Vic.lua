---@type GA_Stealth_C
local M = UnLua.Class("Ability.GameplayAbility")
local Utils = require("Utils")
local Enum = require("Enum")

local CenterOffset = -33
local Extent = UE.FVector(30, 30, 90)

function M:OnInitialized(ActorInfo)
    self.Super.OnInitialized(self, ActorInfo)

    if UE.UStatics.IsShippingBuild() then
        return
    end

    if ActorInfo.PlayerController and ActorInfo.PlayerController:IsValid() then
        self:OnPlayerControllerSet(ActorInfo.PlayerController)
    else
        local Asc = ActorInfo.AbilitySystemComponent
        if not Asc or not Asc:IsValid() then
            Utils.LogError("invalid UDAbilitySystemComponent")
            return
        end

        if not Asc:IsA(UE.UDAbilitySystemComponent) then
            Utils.LogError("AbilitySystemComponent is not UDAbilitySystemComponent")
            return
        end

        Asc.PlayerControllerSet:Add(self, self.OnPlayerControllerSet)
    end
end

---@param PlayerController APlayerController
function M:OnPlayerControllerSet(PlayerController)
    if not PlayerController or not PlayerController:IsValid() then
        Utils.LogError("invalid PlayerController")
        return
    end

    if PlayerController:IsLocalController() then
        UnLua.ConsoleCommand.Add("Ability.StealthVic.DrawBox", "Draw sealth box overlap shape duration", function(Args)
            local Duration = tonumber(Args[1]) or 10
            local Avatar = self:GetAvatarActorFromActorInfo()
            if not Avatar or not Avatar:IsValid() then
                Utils.LogError("invalid Avatar")
                return
            end


            local Center = Avatar:K2_GetActorLocation() + Avatar:GetActorForwardVector() * CenterOffset
            local Rotation = Avatar:K2_GetActorRotation()
            UE.UKismetSystemLibrary.DrawDebugBox(self, Center, Extent, UE.FLinearColor(1, 0, 0), Rotation, Duration, 2)
        end)
    end
end

function M:K2_ActivateAbility()
    self.Super.K2_ActivateAbility(self)

    self.OverlapActors = UE.TArray(UE.AActor)
    if not self:K2_CommitAbility() then
        self:K2_EndAbility()
        return
    end
    
    local bIsOk, Avatar = xpcall(function(self)
        local OutAvatar = self:GetAvatarActorFromActorInfo()
        if not OutAvatar or not OutAvatar:IsValid() then
            error("invalid Avatar")
        end

        if not OutAvatar:IsA(UE.ACharacter) then
            error("Avatar is not ACharacter")
        end

        return OutAvatar
    end, function(Err)
        Utils.LogError(Err)
    end, self)

    if not bIsOk then
        self:K2_EndAbility()
        return
    end

    local BoxCenter = Avatar:K2_GetActorLocation() + Avatar:GetActorForwardVector() * CenterOffset
    local ObjectTypes = UE.TArray(UE.EObjectTypeQuery)
    ObjectTypes:Add(UE.EObjectTypeQuery.Pawn)

    local ActorsToIgnore = UE.TArray(UE.AActor)
    ActorsToIgnore:Add(Avatar)

    self.BoxOverlap = UE.UAbilityTask_TickBoxOverlap.BoxOverlap(self, BoxCenter, Extent, ObjectTypes, ActorsToIgnore)
    if self.BoxOverlap and self.BoxOverlap:IsValid() then
        self.BoxOverlap.Overlap:Add(self, self.OnOverlap)
        self.BoxOverlap:ReadyForActivation()
        Avatar.OnCharacterMovementUpdated:Add(self, self.CharacterMovementUpdated)
    end
end

---@param bIsOverlap boolean
---@param OverlapActors TArray<AActor>
function M:OnOverlap(bIsOverlap, OverlapActors)
    local Avatar = self:GetAvatarActorFromActorInfo()
    if not Avatar or not Avatar:IsValid() then
        Utils.LogError("invalid Avatar")
        return
    end

    self.OverlapActors = self.OverlapActors or UE.TArray(UE.AActor)

    local RemoveOverlapActors = UE.TArray(UE.AActor)
    for _, Actor in pairs(self.OverlapActors) do
        if not bIsOverlap or not OverlapActors:Contains(Actor) then
            Actor:SetStealthTarget(nil)
            RemoveOverlapActors:Add(Actor)
        else
            OverlapActors:RemoveItem(Actor)
        end
    end

    for _, Actor in pairs(RemoveOverlapActors) do
        self.OverlapActors:RemoveItem(Actor)
    end

    for _, Actor in pairs(OverlapActors) do
        if not Utils.HasImplementInterface(Actor, Enum.BPInterface.BI_Stealth) then
            goto continue
        end

        if self.OverlapActors:Contains(Actor) then
            goto continue
        end

        self.OverlapActors:AddUnique(Actor)
        Actor:SetStealthTarget(Avatar)

        ::continue::
    end
end

---@param DeltaSeconds float
---@param OldLocation FVector
---@param OldVelocity FVector
function M:CharacterMovementUpdated(DeltaSeconds, OldLocation, OldVelocity)
    if not self.BoxOverlap or not self.BoxOverlap:IsValid() then
        return
    end

    local Avatar = self:GetAvatarActorFromActorInfo()
    if not Avatar or not Avatar:IsValid() then
        Utils.LogError("invalid Avatar")
        return
    end

    local Location = Avatar:K2_GetActorLocation()
    if Location ~= OldLocation then
        local BoxCenter = Location + Avatar:GetActorForwardVector() * CenterOffset
        self.BoxOverlap:UpdateBoxCenter(BoxCenter)
    end
end

return M