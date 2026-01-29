local Utils = {}

function Utils.MakeGameplayTagContainer(TagContainer, Tags)
    if type(Tags) ~= "table" then
        return
    end

    if type(TagContainer) ~= "userdata" then
        return
    end

    local mt = getmetatable(TagContainer)
    if not mt or mt.__name ~= "FGameplayTagContainer" then
        return
    end

    for _, Tag in ipairs(Tags) do
        if Tag and UE.UBlueprintGameplayTagLibrary.IsGameplayTagValid(Tag) then
            UE.UBlueprintGameplayTagLibrary.AddGameplayTag(TagContainer, Tag)
        end
    end
end

function Utils.TagExactMatch(Tag, OtherTag)
    return UE.UBlueprintGameplayTagLibrary.MatchesTag(Tag, OtherTag, true)
end

return Utils