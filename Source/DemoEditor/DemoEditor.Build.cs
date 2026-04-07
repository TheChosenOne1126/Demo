using UnrealBuildTool;

public class DemoEditor : ModuleRules
{
	public DemoEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"UnrealEd",
			"ToolMenus",
			"LevelEditor",
			"Demo",
			"GameplayTags",
			"ContentBrowserData",
			"ContentBrowser",
			"UMG",
			"UMGEditor",
			"UnLua",
			"Kismet"
		});
	}
}

