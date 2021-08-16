// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExamplePluginModule.h"

#include "LevelEditor.h"

#include "ExamplePluginCommands.h"
#include "UI/ExamplePluginStyle.h"
#include "UI/SearchEverywhereWindow.h"
#include "UI/SearchEverywhereWidget.h"
#include "Multithreading/Searcher.h"

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
	FMessageEndpoint::SafeRelease(MessageEndpoint);
}

void FExamplePluginModule::PluginButtonClicked()
{
	TSharedPtr<SSearchEverywhereWindow> ExistingWindow = PluginWindow.Pin();
	if (ExistingWindow.IsValid() && ExistingWindow->IsActive())
	{
		ExistingWindow->BringToFront();
	}
	else
	{
		TWeakPtr<SWidget> PreviousFocusedUserWidget = FSlateApplication::Get().GetUser(LastKeyboardUserIndex)->
		                                                                       GetFocusedWidget();
		ExistingWindow =
			SNew(SSearchEverywhereWindow, PreviousFocusedUserWidget)
			.PreviousSearchRequest(PreviousSearchRequest);
		FSlateApplication::Get().AddWindow(ExistingWindow.ToSharedRef());
	}
	PluginWindow = ExistingWindow;
}

void FExamplePluginModule::OnApplicationPreInputKeyDownListener(const FKeyEvent& InKeyEvent)
{
	// todo save many users
	const FInputChord CheckChord(InKeyEvent.GetKey(),
	                             EModifierKey::FromBools(InKeyEvent.IsControlDown(), InKeyEvent.IsAltDown(),
	                                                     InKeyEvent.IsShiftDown(), InKeyEvent.IsCommandDown()));
	if (FExamplePluginCommands::Get().OpenPluginWindow->HasActiveChord(CheckChord))
	{
		LastKeyboardUserIndex = InKeyEvent.GetUserIndex();
		LastKeyboardUserInput = InKeyEvent.GetKey();
	}
}

void FExamplePluginModule::HandleFoundWords(const FResultItemFoundMsg& InMessage,
                                            const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& InContext)
{
	if (const TSharedPtr<SSearchEverywhereWindow> Window = PluginWindow.Pin())
	{
		Window->GetSearchEverywhereWidget()->UpdateShownResults();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExamplePluginModule, ExamplePlugin)
