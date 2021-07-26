// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExamplePluginModule.h"

#include "Multithreading/ExampleAsyncTask.h"
#include "UI/ExamplePluginStyle.h"
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
#include "Multithreading/CallbackHandler.h"
#include "UI/SearchEverywhereWidget.h"
#include "UI/SearchEverywhereWindow.h"
#include "Multithreading/Searcher.h"
#include "Dictionary/Dictionary.h"
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
	// CallbackHandler->SetWindowPtr(ExamplePluginWindow);
	// Append to level editor module so that shortcuts are accessible in level editor
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditorModule.GetGlobalLevelEditorActions()->Append(PluginCommands.ToSharedRef());
	//todo add to asset editor
	// Searcher->OnNewDataFound().BindThreadSafeSP(CallbackHandler, &FCallbackHandler::OnCallback); //todo maybe bind static
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
}

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
		TWeakPtr<SWidget> PreviousFocusedUserWidget = FSlateApplication::Get().GetUser(LastKeyboardUser)->
		                                                                       GetFocusedWidget();
		ExistingWindow = SNew(SSearchEverywhereWindow, PreviousFocusedUserWidget, PluginCommands, Searcher);
		FSlateApplication::Get().AddWindow(ExistingWindow.ToSharedRef());
	}
	ExamplePluginWindow = ExistingWindow;
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



#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExamplePluginModule, ExamplePlugin)
