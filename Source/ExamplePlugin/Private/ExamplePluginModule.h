// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Multithreading/CallbackHandler.h"
#include "Multithreading/Searcher.h"

class FToolBarBuilder;
class FMenuBuilder;

class FExamplePluginModule final : public IModuleInterface,
                                   public TSharedFromThis<FExamplePluginModule, ESPMode::ThreadSafe>
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	void OnApplicationPreInputKeyDownListener(const FKeyEvent& InKeyEvent);
private:
	uint32 LastKeyboardUser = 0;
	FKey LastKeyboardUserInput;
	TSharedPtr<class FUICommandList> PluginCommands;
	TWeakPtr<class SSearchEverywhereWindow> ExamplePluginWindow; // todo maybe not import but declaration
	FDelegateHandle OnApplicationPreInputKeyDownListenerHandle;
	const static int32 ResultChunkSize = 100;
	TSharedRef<FCallbackHandler, ESPMode::ThreadSafe> CallbackHandler = MakeShared<
		FCallbackHandler, ESPMode::ThreadSafe>(ExamplePluginWindow);
	TSharedRef<FSearcher> Searcher = MakeShared<FSearcher>(ResultChunkSize,
	                                                       CallbackHandler->GetMessageEndpoint());
};
