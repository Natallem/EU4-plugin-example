// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExamplePlugin.h"
#include "ExamplePluginStyle.h"
#include "ExamplePluginCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName ExamplePluginTabName("ExamplePlugin");

#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void FExamplePluginModule::StartupModule()
{
	
	FExamplePluginStyle::Initialize();
	FExamplePluginStyle::ReloadTextures();

	FExamplePluginCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FExamplePluginCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FExamplePluginModule::PluginButtonClicked),
		FCanExecuteAction());

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ExamplePluginTabName, FOnSpawnTab::CreateRaw(this, &FExamplePluginModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FExamplePluginTabTitle", "ExamplePlugin"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
	
	// Append to level editor module so that shortcuts are accessible in level editor
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditorModule.GetGlobalLevelEditorActions()->Append(PluginCommands.ToSharedRef());
	
}

void FExamplePluginModule::ShutdownModule()
{

	FExamplePluginStyle::Shutdown();

	FExamplePluginCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ExamplePluginTabName);
}

TSharedRef<SDockTab> FExamplePluginModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FExamplePluginModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("ExamplePlugin.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FExamplePluginModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ExamplePluginTabName);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExamplePluginModule, ExamplePlugin)