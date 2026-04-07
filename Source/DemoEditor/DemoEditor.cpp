#include "DemoEditor.h"
#include "GameplayTagsManager.h"
#include "ToolMenus.h"
#include "Styling/AppStyle.h"
#include "Misc/ScopedSlowTask.h"
#include "ContentBrowserMenuContexts.h"
#include "WidgetBlueprint.h"
#include "UnLuaInterface.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

#define LOCTEXT_NAMESPACE "DemoEditorModule"

void FDemoEditorModule::StartupModule()
{
	RegisterStartupCallbackHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]() -> void
	{
		RegisterPlayToolbarButton();
		RegisterAssetContextMenu();
	}));
}

void FDemoEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(RegisterStartupCallbackHandle);
	UToolMenus::UnregisterOwner(this);
}

void FDemoEditorModule::RegisterPlayToolbarButton()
{
	UToolMenu* PlayToolBar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	if (!PlayToolBar)
	{
		return;
	}

	FToolMenuSection& Section = PlayToolBar->FindOrAddSection("DemoTools");

	FToolMenuEntry SimpleButtonEntry = FToolMenuEntry::InitToolBarButton(
		"ExportGameplayTags",
		FUIAction(
			FExecuteAction::CreateStatic(ExportGameplayTagsToLua),
			FCanExecuteAction::CreateLambda([]() -> bool
			{
				return true;
			})
		),
		LOCTEXT("Label_ExportGameplayTags", "ExportGameplayTags"),
		LOCTEXT("Tooltip_ExportGameplayTags", "Click to export GameplayTags to lua file"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "FontEditor.ExportPage", "FontEditor.ExportPage.Small")
	);
	SimpleButtonEntry.StyleNameOverride = "CalloutToolbar";
	
	Section.AddEntry(SimpleButtonEntry);

	// ---------- ComboButton (dropdown menu, like UnLua's style) ----------
	// Uncomment the following block if you want a dropdown combo button instead:
	/*
	FToolMenuEntry ComboEntry = FToolMenuEntry::InitComboButton(
		"DemoComboButton",
		FUIAction(),
		FOnGetContent::CreateLambda([this](void) -> TSharedRef<SWidget>
		{
			FMenuBuilder MenuBuilder(true, nullptr);

			MenuBuilder.AddMenuEntry(
				LOCTEXT("Option1", "Option 1"),
				LOCTEXT("Option1_Tooltip", "First option"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([]()
				{
					UE_LOG(LogTemp, Log, TEXT("[Demo] Option 1 selected"));
				}))
			);

			MenuBuilder.AddMenuEntry(
				LOCTEXT("Option2", "Option 2"),
				LOCTEXT("Option2_Tooltip", "Second option"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([]()
				{
					UE_LOG(LogTemp, Log, TEXT("[Demo] Option 2 selected"));
				}))
			);

			return MenuBuilder.MakeWidget();
		}),
		LOCTEXT("DemoCombo_Label", "Demo"),
		LOCTEXT("DemoCombo_Tooltip", "Demo combo button"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "PlayWorld.PlayInViewport")
	);

	Section.AddEntry(ComboEntry);
	*/
}

void FDemoEditorModule::ExportGameplayTagsToLua()
{
	FGameplayTagContainer AllTags;
	UGameplayTagsManager::Get().RequestAllGameplayTags(AllTags, true);

	const TArray<FGameplayTag>& AllTagList = AllTags.GetGameplayTagArray();

	// Total steps: filter each tag + generate content + save file
	FScopedSlowTask SlowTask(AllTagList.Num() + 2, LOCTEXT("ExportingGameplayTags", "Exporting GameplayTags to Lua..."));
	SlowTask.MakeDialog();

	// Step 1: Filter project tags
	TArray<FGameplayTag> ProjectTags;
	for (const FGameplayTag& Tag : AllTagList)
	{
		if (SlowTask.ShouldCancel())
			return;

		SlowTask.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Filtering: %s"), *Tag.ToString())));

		TSharedPtr<FGameplayTagNode> TagNode = UGameplayTagsManager::Get().FindTagNode(Tag);
		if (!TagNode.IsValid())
		{
			continue;
		}

		const FName SourceName = TagNode->GetFirstSourceName();
		if (!SourceName.ToString().Contains(FString(FApp::GetProjectName())) && !SourceName.ToString().Contains(TEXT("/Game")))
		{
			continue;
		}

		ProjectTags.Add(Tag);
	}

	if (SlowTask.ShouldCancel())
		return;

	// Step 2: Generate lua content
	SlowTask.EnterProgressFrame(1, LOCTEXT("GeneratingLuaContent", "Generating Lua content..."));

	FString LuaContent = TEXT("--- Auto-generated GameplayTags ---\n");
	LuaContent += TEXT("local RequestGameplayTag = UE.FGameplayTag.RequestGameplayTag\n");
	LuaContent += TEXT("local GameplayTags = {\n");
    	
	for (int i = 0; i < ProjectTags.Num(); ++i)
	{
		FString SafeName = ProjectTags[i].ToString().Replace(TEXT("."), TEXT("_"));
		SafeName = SafeName.Replace(TEXT("("), TEXT("_"));
		SafeName = SafeName.Replace(TEXT(")"), TEXT(""));
		LuaContent += FString::Printf(TEXT("    %s = RequestGameplayTag(\"%s\")"), *SafeName, *ProjectTags[i].ToString());
    
		if (i < ProjectTags.Num() - 1)
		{
			LuaContent += TEXT(",");
		}
		LuaContent += TEXT("\n");
	}
    	
	LuaContent += TEXT("}\n");
	LuaContent += TEXT("return GameplayTags");

	if (SlowTask.ShouldCancel())
		return;

	// Step 3: Save file
	SlowTask.EnterProgressFrame(1, LOCTEXT("SavingLuaFile", "Saving GameplayTags.lua..."));

	const FString FilePath = FPaths::ProjectContentDir() / TEXT("Script/GameplayTags.lua");
    
	if (FFileHelper::SaveStringToFile(LuaContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		FPlatformProcess::ExploreFolder(*FPaths::GetPath(FilePath));
	}
}

void FDemoEditorModule::RegisterAssetContextMenu()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
	if (!Menu)
	{
		return;
	}

	FToolMenuSection& Section = Menu->FindOrAddSection("UnLuaWidget");
	Section.Label = LOCTEXT("UnLuaWidget_SectionLabel", "UnLua");

	Section.AddDynamicEntry("BindWidgetToUnLua", FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
	{
		const UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
		if (!Context || Context->SelectedAssets.Num() == 0)
		{
			return;
		}

		// Only show for WidgetBlueprint assets
		bool bHasWidgetBlueprint = false;
		for (const FAssetData& Asset : Context->SelectedAssets)
		{
			if (Asset.AssetClassPath == UWidgetBlueprint::StaticClass()->GetClassPathName())
			{
				bHasWidgetBlueprint = true;
				break;
			}
		}

		if (!bHasWidgetBlueprint)
		{
			return;
		}

		InSection.AddMenuEntry(
			"BindWidgetToUnLua",
			LOCTEXT("BindWidgetToUnLua_Label", "Bind to UnLua & Export"),
			LOCTEXT("BindWidgetToUnLua_Tooltip", "Implement IUnLuaInterface and export widget path to Lua file"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.WidgetBlueprint"),
			FUIAction(FExecuteAction::CreateLambda([SelectedAssets = Context->SelectedAssets]()
			{
				BindWidgetToUnLua(SelectedAssets);
				ExportWidgetPathsToLua(SelectedAssets);
			}))
		);
	}));
}

void FDemoEditorModule::BindWidgetToUnLua(const TArray<FAssetData>& SelectedAssets)
{
	for (const FAssetData& AssetData : SelectedAssets)
	{
		if (AssetData.AssetClassPath != UWidgetBlueprint::StaticClass()->GetClassPathName())
		{
			continue;
		}

		UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(AssetData.GetAsset());
		if (!IsValid(WidgetBP))
		{
			continue;
		}

		// Skip if already implements IUnLuaInterface
		if (WidgetBP->GeneratedClass && WidgetBP->GeneratedClass->ImplementsInterface(UUnLuaInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("[DemoEditor] '%s' already implements IUnLuaInterface, skipping."), *WidgetBP->GetName());
			continue;
		}

		const bool bOk = FBlueprintEditorUtils::ImplementNewInterface(WidgetBP, UUnLuaInterface::StaticClass()->GetClassPathName());
		if (!bOk)
		{
			UE_LOG(LogTemp, Error, TEXT("[DemoEditor] Failed to implement IUnLuaInterface on '%s'."), *WidgetBP->GetName());
			continue;
		}

		// Set GetModuleName default value: /Game/UI/WBP_Xxx -> UI.WBP_Xxx
		const FBPInterfaceDescription* InterfaceDesc = WidgetBP->ImplementedInterfaces.FindByPredicate([](const FBPInterfaceDescription& Desc)
		{
			return Desc.Interface == UUnLuaInterface::StaticClass();
		});

		if (InterfaceDesc)
		{
			// GetModuleName should return a Lua module path like "UI.WBP_MainMenu"
			FString LuaModuleName = FString::Printf(TEXT("UI.%s"), *WidgetBP->GetName());

			// Graphs[1] = GetModuleName, Nodes[1] = FunctionResult, Pins[1] = ReturnValue
			if (InterfaceDesc->Graphs.IsValidIndex(1)
				&& InterfaceDesc->Graphs[1]->Nodes.IsValidIndex(1)
				&& InterfaceDesc->Graphs[1]->Nodes[1]->Pins.IsValidIndex(1))
			{
				InterfaceDesc->Graphs[1]->Nodes[1]->Pins[1]->DefaultValue = LuaModuleName;
			}

			// Generate template Lua file: Content/Script/UI/WBP_Xxx.lua
			const FString LuaFilePath = FPaths::ProjectContentDir() / TEXT("Script/UI") / (WidgetBP->GetName() + TEXT(".lua"));
			if (!FPaths::FileExists(LuaFilePath))
			{
				FString LuaTemplate;
				LuaTemplate += TEXT("local M = UnLua.Class()\n");
				LuaTemplate += TEXT("\n");
				LuaTemplate += FString::Printf(TEXT("--function M:Construct()\n"));
				LuaTemplate += FString::Printf(TEXT("--end\n"));
				LuaTemplate += TEXT("\n");
				LuaTemplate += FString::Printf(TEXT("--function M:Destruct()\n"));
				LuaTemplate += FString::Printf(TEXT("--end\n"));
				LuaTemplate += TEXT("\n");
				LuaTemplate += TEXT("return M\n");

				if (FFileHelper::SaveStringToFile(LuaTemplate, *LuaFilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
				{
					UE_LOG(LogTemp, Log, TEXT("[DemoEditor] Generated Lua file: %s"), *LuaFilePath);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[DemoEditor] Failed to generate Lua file: %s"), *LuaFilePath);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[DemoEditor] Lua file already exists, skipping: %s"), *LuaFilePath);
			}
		}

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBP);
		FKismetEditorUtilities::CompileBlueprint(WidgetBP);
		WidgetBP->MarkPackageDirty();

		UE_LOG(LogTemp, Log, TEXT("[DemoEditor] Successfully bound '%s' to UnLua."), *WidgetBP->GetName());
	}
}

void FDemoEditorModule::ExportWidgetPathsToLua(const TArray<FAssetData>& SelectedAssets)
{
	// Read existing content from file to preserve previously exported entries
	const FString FilePath = FPaths::ProjectContentDir() / TEXT("Script/UI/WidgetPaths.lua");

	TMap<FString, FString> ExistingEntries;
	FString ExistingContent;
	if (FFileHelper::LoadFileToString(ExistingContent, *FilePath))
	{
		// Parse existing entries: key = value lines
		TArray<FString> Lines;
		ExistingContent.ParseIntoArrayLines(Lines);
		for (const FString& Line : Lines)
		{
			// Match pattern: SafeName = "/Game/..."
			FString Trimmed = Line.TrimStartAndEnd();
			if (Trimmed.StartsWith(TEXT("---")) || Trimmed.StartsWith(TEXT("local"))
				|| Trimmed.StartsWith(TEXT("return")) || Trimmed == TEXT("{") || Trimmed == TEXT("}"))
			{
				continue;
			}
			int32 EqIdx = INDEX_NONE;
			if (Trimmed.FindChar(TEXT('='), EqIdx))
			{
				FString Key = Trimmed.Left(EqIdx).TrimStartAndEnd();
				FString Value = Trimmed.Mid(EqIdx + 1).TrimStartAndEnd();
				Value.RemoveFromEnd(TEXT(","));
				if (!Key.IsEmpty() && !Value.IsEmpty())
				{
					ExistingEntries.Add(Key, Value);
				}
			}
		}
	}

	// Add new entries from selected assets
	for (const FAssetData& AssetData : SelectedAssets)
	{
		if (AssetData.AssetClassPath != UWidgetBlueprint::StaticClass()->GetClassPathName())
		{
			continue;
		}

		// e.g. /Game/UI/WBP_MainMenu -> Key=WBP_MainMenu, Value="/Game/UI/WBP_MainMenu.WBP_MainMenu"
		FString PackagePath = AssetData.PackageName.ToString();
		FString AssetName = AssetData.AssetName.ToString();
		FString SafeName = AssetName.Replace(TEXT(" "), TEXT("_"));
		FString FullPath = FString::Printf(TEXT("\"%s.%s\""), *PackagePath, *AssetName);
		ExistingEntries.Add(SafeName, FullPath);
	}

	// Generate lua file
	FString LuaContent = TEXT("--- Auto-generated Widget Paths ---\n");
	LuaContent += TEXT("local WidgetPaths = {\n");

	TArray<FString> Keys;
	ExistingEntries.GetKeys(Keys);
	Keys.Sort();

	for (int32 i = 0; i < Keys.Num(); ++i)
	{
		LuaContent += FString::Printf(TEXT("    %s = %s"), *Keys[i], *ExistingEntries[Keys[i]]);
		if (i < Keys.Num() - 1)
		{
			LuaContent += TEXT(",");
		}
		LuaContent += TEXT("\n");
	}

	LuaContent += TEXT("}\n");
	LuaContent += TEXT("return WidgetPaths\n");

	if (FFileHelper::SaveStringToFile(LuaContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogTemp, Log, TEXT("[DemoEditor] Widget paths exported to: %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[DemoEditor] Failed to save widget paths to: %s"), *FilePath);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDemoEditorModule, DemoEditor)

