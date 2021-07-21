// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CallbackHandler.h"
#include "SearchEverywhereWindow.h"
#include "Modules/ModuleManager.h"
#include "FeedbackContextEditor.h"
#include "Toolkits/GlobalEditorCommonCommands.h"
#include "Multithreading/Searcher.h"

class FToolBarBuilder;
class FMenuBuilder;

class FExamplePluginModule : public IModuleInterface, public TSharedFromThis<FExamplePluginModule, ESPMode::ThreadSafe>
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	void OnApplicationPreInputKeyDownListener(const FKeyEvent& InKeyEvent);

	static void OnNewDataFound();

private:

	// TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	FFeedbackContextEditor e; //todo delete
	uint32 LastKeyboardUser = 0;
	FKey LastKeyboardUserInput;
	TSharedPtr<class FUICommandList> PluginCommands;
	TWeakPtr<SSearchEverywhereWindow> ExamplePluginWindow; // todo maybe not import but declaration
	TWeakPtr<SWindow> BuildProgressWindow; // todo maybe not import but declaration
	FDelegateHandle OnApplicationPreInputKeyDownListenerHandle;
	FEvent * HelloWindowEvent = nullptr;
	FSearcher * Searcher = nullptr;
	TSharedRef<class FCallbackHandler, ESPMode::ThreadSafe> CallbackHandler = MakeShared<FCallbackHandler, ESPMode::ThreadSafe>();;

};

