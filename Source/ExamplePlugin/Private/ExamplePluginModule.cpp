// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExamplePluginModule.h"

#include "LevelEditor.h"

#include "ExamplePluginCommands.h"
#include "Interfaces/IMainFrameModule.h"
#include "UI/ExamplePluginStyle.h"
#include "UI/SearchEverywhereWindow.h"
#include "UI/SearchEverywhereWidget.h"
#include "Multithreading/Searcher.h"


class IMainFrameModule;
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
	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	MainFrameModule.OnMainFrameCreationFinished().AddRaw(this, &FExamplePluginModule::OnMainFrameLoaded);
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
	if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
	{
		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		MainFrameModule.OnMainFrameCreationFinished().RemoveAll(this);
	}
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
	const TSharedRef<const FInputChord> CommandChord = FExamplePluginCommands::Get().OpenPluginWindow->GetActiveChord(
		EMultipleKeyBindingIndex::Primary);
	if (CommandChord->Key == InKeyEvent.GetKey())
	{
		LastKeyboardUserIndex = InKeyEvent.GetUserIndex();
	}
}

void FExamplePluginModule::OnMainFrameLoaded(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow)
{
	FPropertyHolder::Get();
}

void FExamplePluginModule::HandleFoundWords(const FItemFoundMsg& InMessage,
                                            const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& InContext)
{
	if (const TSharedPtr<SSearchEverywhereWindow> Window = PluginWindow.Pin())
	{
		Window->GetSearchEverywhereWidget()->ProcessMessage(InMessage);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExamplePluginModule, ExamplePlugin)
