// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExamplePlugin.h"
#include "ExamplePluginStyle.h"
#include "ExamplePluginCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/STextEntryPopup.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/Text/SRichTextBlock.h"

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

	// FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ExamplePluginTabName,
	                                                  // FOnSpawnTab::CreateRaw(
		                                                  // this, &FExamplePluginModule::OnSpawnPluginTab))
	                        // .SetDisplayName(LOCTEXT("FExamplePluginTabTitle", "ExamplePlugin"))
	                        // .SetMenuType(ETabSpawnerMenuType::Hidden);

	// Append to level editor module so that shortcuts are accessible in level editor
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditorModule.GetGlobalLevelEditorActions()->Append(PluginCommands.ToSharedRef());
}

void FExamplePluginModule::ShutdownModule()
{
	FExamplePluginStyle::Shutdown();

	FExamplePluginCommands::Unregister();

	// FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ExamplePluginTabName);
}

/*
TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	FText LabelNameText;
	FText ToolTipText;
	if (TabIdentifier == FName("AllExampleTab"))
	{
		LabelNameText = LOCTEXT("AllExampleTab", "All");
		ToolTipText = LOCTEXT("AllExampleTabToolTip", "Shows all content");
	}
	else if (TabIdentifier == FName("ClassesExampleTab"))
	{
		LabelNameText = LOCTEXT("ClassesExampleTab", "Classes");
		ToolTipText = LOCTEXT("ClassesExampleTabToolTip", "Shows classes only");
	}
	else if (TabIdentifier == FName("FilesExampleTab"))
	{
		LabelNameText = LOCTEXT("FilesExampleTab", "Files");
		ToolTipText = LOCTEXT("FilesExampleTabToolTip", "Shows files only");
	}
	else
	{
		ensure(false);
		return SNew(SDockTab);
	}
	return SNew(SDockTab)
			.Label(LabelNameText)
			.ToolTipText(ToolTipText)
			.Clipping(EWidgetClipping::ClipToBounds)
	[
		SNew(STextEditTest)
	];
}

static TSharedPtr<FTabManager> TestSuite1TabManager;

TSharedRef<SDockTab> FExamplePluginModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FExamplePluginModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("ExamplePlugin.cpp"))
	);

	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("TestSuite2_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->Split
			(
				FTabManager::NewStack()
				->AddTab("AllExampleTab", ETabState::OpenedTab)
				->AddTab("ClassesExampleTab", ETabState::OpenedTab)
				->AddTab("FilesExampleTab", ETabState::OpenedTab)
			)
		);

	TSharedRef<SDockTab> TestSuite1Tab =
		SNew(SDockTab)
		.TabRole(ETabRole::MajorTab)
		.Label(LOCTEXT("FExamplePluginTabTitle", "ExamplePlugin"))
		.ToolTipText(LOCTEXT("FExamplePluginTabTip", "The tip for the example plugin main tab."));

	TestSuite1TabManager = FGlobalTabmanager::Get()->NewTabManager(TestSuite1Tab);

	TestSuite1TabManager->RegisterTabSpawner("AllExampleTab",
	                                         FOnSpawnTab::CreateStatic(&SpawnTab, FName("AllExampleTab")))
	                    .SetDisplayName(NSLOCTEXT("ExampleTabName", "AllExampleTab", "All"));

	TestSuite1TabManager->RegisterTabSpawner("ClassesExampleTab",
	                                         FOnSpawnTab::CreateStatic(&SpawnTab, FName("ClassesExampleTab")))
	                    .SetDisplayName(NSLOCTEXT("ExampleTabName", "ClassesExampleTab", "Classes"));

	TestSuite1TabManager->RegisterTabSpawner("FilesExampleTab",
	                                         FOnSpawnTab::CreateStatic(&SpawnTab, FName("FilesExampleTab")))
	                    .SetDisplayName(NSLOCTEXT("ExampleTabName", "FilesExampleTab", "Files"));

	TestSuite1Tab->SetContent
	(
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			TestSuite1TabManager->RestoreFrom(Layout, SpawnTabArgs.GetOwnerWindow()).ToSharedRef()
		]
	);
	return TestSuite1Tab;
}
*/

void FExamplePluginModule::PluginButtonClicked()
{
	TSharedPtr<SSearchEverywhereWindow> ExistingWindow = ExamplePluginWindow.Pin();
	if (ExistingWindow.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("EP : Bring to front window"));
		ExistingWindow->BringToFront();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("EP : Created new window"));

		TSharedPtr<SWindow> ParentWindow;
		if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
		{
			IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
			ParentWindow = MainFrame.GetParentWindow();
			FVector2D t = ParentWindow->GetSizeInScreen();
			int64 d = 10;
		}
		else
		{
			ensureAlwaysMsgf(false, TEXT("MainFrame module is not loaded"));
		}
		FVector2D tt = ParentWindow->GetClientSizeInScreen();
		FVector2D tt1 = ParentWindow->GetDesiredSizeDesktopPixels();
		FVector2D tt2 = ParentWindow->GetViewportSize();
		FVector2D tt3 = ParentWindow->GetSizeInScreen();
		FVector2D tt4 = ParentWindow->GetInitialDesiredSizeInScreen();
		float tt5 = ParentWindow->GetDPIScaleFactor();

		ExistingWindow = SNew(SSearchEverywhereWindow); 
		// ExistingWindow->SetAllowFastUpdate(true); // todo what is it?
		FVector2D tt_ = ExistingWindow->GetClientSizeInScreen();
		FVector2D tt_1 = ExistingWindow->GetDesiredSizeDesktopPixels();
		FVector2D tt_2 = ExistingWindow->GetViewportSize();
		FVector2D tt_3 = ExistingWindow->GetSizeInScreen();
		FVector2D tt_4 = ExistingWindow->GetInitialDesiredSizeInScreen();
		/*float tt_5 = */
		// ExistingWindow->SetDPIScaleFactor(3);

		// GEditor->EditorAddModalWindow(ExistingWindow.ToSharedRef());
		// GEditor->size
		// FSlateApplication::Get().AddModalWindow(ExistingWindow.ToSharedRef(), ParentWindow);
		FSlateApplication::Get().AddWindow(ExistingWindow.ToSharedRef());
	
	}

	ExamplePluginWindow = ExistingWindow;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExamplePluginModule, ExamplePlugin)
