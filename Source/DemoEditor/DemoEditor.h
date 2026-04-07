#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FDemoEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	static void RegisterPlayToolbarButton();
	static void RegisterAssetContextMenu();
	
	static void ExportGameplayTagsToLua();

	static void BindWidgetToUnLua(const TArray<FAssetData>& SelectedAssets);
	static void ExportWidgetPathsToLua(const TArray<FAssetData>& SelectedAssets);
	
	FDelegateHandle RegisterStartupCallbackHandle;
};

