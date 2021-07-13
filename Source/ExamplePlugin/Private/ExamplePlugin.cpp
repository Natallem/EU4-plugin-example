// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExamplePlugin.h"
#include "ExamplePluginStyle.h"
#include "ExamplePluginCommands.h"
#include "LevelEditor.h"
#include "Widgets/Layout/SScrollBox.h"
#include "ToolMenus.h"
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
	OnApplicationPreInputKeyDownListenerHandle = FSlateApplication::Get().OnApplicationPreInputKeyDownListener().AddRaw(
		this, &FExamplePluginModule::OnApplicationPreInputKeyDownListener);

	PluginCommands = MakeShareable(new FUICommandList);
	/*IMainFrameModule::Get().GetMainFrameCommandBindings()->MapAction(
		FExamplePluginCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FExamplePluginModule::PluginButtonClicked),
		FCanExecuteAction());*/

	PluginCommands->MapAction(
		FExamplePluginCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FExamplePluginModule::PluginButtonClicked),
		FCanExecuteAction());

	// Append to level editor module so that shortcuts are accessible in level editor
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditorModule.GetGlobalLevelEditorActions()->Append(PluginCommands.ToSharedRef());
	// IMainFrameModule::Get().GetMainFrameCommandBindings()->Append(PluginCommands.ToSharedRef());
}

void FExamplePluginModule::ShutdownModule()
{
	FExamplePluginStyle::Shutdown();
	if (OnApplicationPreInputKeyDownListenerHandle.IsValid())
	{
		FSlateApplication::Get().OnApplicationPreInputKeyDownListener().Remove(
			OnApplicationPreInputKeyDownListenerHandle);
		OnApplicationPreInputKeyDownListenerHandle = FDelegateHandle();
	}
	FExamplePluginCommands::Unregister();
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
		TSharedPtr<FSlateUser> currentUser = FSlateApplication::Get().GetUser(LastKeyboardUser);
		TWeakPtr<SWidget> pr = currentUser->GetFocusedWidget();
	
		ExistingWindow = SNew(SSearchEverywhereWindow, pr, PluginCommands);
		FSlateApplication::Get().AddWindow(ExistingWindow.ToSharedRef());
		// const TArray<TSharedRef<SWindow>> t = FSlateApplication::Get().GetTopLevelWindows();
		// void* t2 = FSlateApplication::Get().GetMouseCaptureWindow();
	}


	ExamplePluginWindow = ExistingWindow;
}

void FExamplePluginModule::OnApplicationPreInputKeyDownListener(const FKeyEvent& InKeyEvent)
{
	// todo save many users
	const FInputChord CheckChord( InKeyEvent.GetKey(),  EModifierKey::FromBools(InKeyEvent.IsControlDown(), InKeyEvent.IsAltDown(), InKeyEvent.IsShiftDown(), InKeyEvent.IsCommandDown()) );
	if (FExamplePluginCommands::Get().OpenPluginWindow->HasActiveChord(CheckChord))
	{
		LastKeyboardUser = InKeyEvent.GetUserIndex();
		LastKeyboardUserInput = InKeyEvent.GetKey();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExamplePluginModule, ExamplePlugin)
