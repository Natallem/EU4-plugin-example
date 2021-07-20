// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExamplePlugin.h"

#include "ExampleAsyncTask.h"
#include "ExamplePluginStyle.h"
#include "ExamplePluginCommands.h"
#include "LevelEditor.h"
#include "Widgets/Layout/SScrollBox.h"
#include "ToolMenus.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/STextEntryPopup.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "FeedbackContextEditor.h"
#include "Toolkits/GlobalEditorCommonCommands.h"
#include "ScopedSourceControlProgress.h"
#include "Dialogs/SBuildProgress.h"
#include "Interfaces/IMainFrameModule.h"
#include "ScopedLocalizationServiceProgress.h"
#include "FeedbackContextEditor.h"
#include "CallbackHandler.h"
#include "SearchEverywhereWidget.h"
static const FName ExamplePluginTabName("ExamplePlugin");

#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void FExamplePluginModule::StartupModule()
{
	FExamplePluginStyle::Initialize();
	FExamplePluginStyle::ReloadTextures();

	FExamplePluginCommands::Register();
	OnApplicationPreInputKeyDownListenerHandle = FSlateApplication::Get().OnApplicationPreInputKeyDownListener().AddRaw(
		this, &FExamplePluginModule::OnApplicationPreInputKeyDownListener);

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FExamplePluginCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FExamplePluginModule::PluginButtonClicked),
		FCanExecuteAction());

	// Append to level editor module so that shortcuts are accessible in level editor
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditorModule.GetGlobalLevelEditorActions()->Append(PluginCommands.ToSharedRef());
	//todo add to asset editor
	Searcher = new FSearcher();
	HelloWindowEvent = Searcher->GetSayHelloEvent();
	// CallbackHandler = MakeShared<FCallbackHandler>(new FCallbackHandler());
	Searcher->OnNewDataFound().BindThreadSafeSP(CallbackHandler, &FCallbackHandler::OnCallback);

	// HelloWindowEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);
}


void FExamplePluginModule::ShutdownModule()
{
	FExamplePluginStyle::Shutdown();
	if (FSlateApplication::IsInitialized() && OnApplicationPreInputKeyDownListenerHandle.IsValid())
	{
		FSlateApplication::Get().OnApplicationPreInputKeyDownListener().Remove(
			OnApplicationPreInputKeyDownListenerHandle);
		OnApplicationPreInputKeyDownListenerHandle = FDelegateHandle();
	}
	FExamplePluginCommands::Unregister();
	Searcher->EnsureCompletion();
	delete Searcher;
}

void FExamplePluginModule::PluginButtonClicked()
{
	static bool bBoundCallback = false;
	if (!bBoundCallback)
	{
		// Searcher->OnNewDataFound().BindThreadSafeSP(AsShared(), &FExamplePluginModule::OnNewDataFound);
		bBoundCallback = true;
	}

	static int num = 0;
	++num;
	/*const FVector2D TabListSize(700.0f, 486.0f);

	// Create the contents of the popup
	TSharedRef<SWidget> ActualWidget = SNew(SGlobalTabSwitchingDialog, TabListSize,FInputChord());

	OpenPopupMenu(ActualWidget, TabListSize);*/

	TSharedPtr<SSearchEverywhereWindow> ExistingWindow = ExamplePluginWindow.Pin();
	if (ExistingWindow.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("EP : Bring to front window"));
		ExistingWindow->BringToFront();
	}
	else
	{
		
		UE_LOG(LogTemp, Log, TEXT("EP : Created new window"));
		TWeakPtr<SWidget> PreviousFocusedUserWidget = FSlateApplication::Get().GetUser(LastKeyboardUser)->
		                                                                       GetFocusedWidget();
		ExistingWindow = SNew(SSearchEverywhereWindow, PreviousFocusedUserWidget, PluginCommands);
		FSlateApplication::Get().AddWindow(ExistingWindow.ToSharedRef());
		// auto HttpsRequestCallbackWrapper = MakeShared<
			// FExamplePluginModule, ESPMode::ThreadSafe>(AsShared());
		// Searcher->OnNewDataFound().BindStatic(&FExamplePluginModule::OnNewDataFound);
		// Searcher->OnNewDataFound().BindThreadSafeSP(CallbackHandler,&FCallbackHandler::OnCallback);
	}
	ExamplePluginWindow = ExistingWindow;
	HelloWindowEvent->Trigger();
	// (new FAutoDeleteAsyncTask< ExampleAsyncTask >( num , HelloWindowEvent) )->StartBackgroundTask();
}

void FExamplePluginModule::OnApplicationPreInputKeyDownListener(const FKeyEvent& InKeyEvent)
{
	// todo save many users
	const FInputChord CheckChord(InKeyEvent.GetKey(),
	                             EModifierKey::FromBools(InKeyEvent.IsControlDown(), InKeyEvent.IsAltDown(),
	                                                     InKeyEvent.IsShiftDown(), InKeyEvent.IsCommandDown()));
	if (FExamplePluginCommands::Get().OpenPluginWindow->HasActiveChord(CheckChord))
	{
		LastKeyboardUser = InKeyEvent.GetUserIndex();
		LastKeyboardUserInput = InKeyEvent.GetKey();
	}
}

void FExamplePluginModule::OnNewDataFound()
{
	int x = 10;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExamplePluginModule, ExamplePlugin)
