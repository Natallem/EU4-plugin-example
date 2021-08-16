// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "InputCoreTypes.h"
#include "Multithreading/CallbackHandler.h"
#include "Multithreading/Searcher.h"

struct FKeyEvent;
class FToolBarBuilder;
class FMenuBuilder;
class FUICommandList;
class SSearchEverywhereWindow;

class FExamplePluginModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void PluginButtonClicked();
	void OnApplicationPreInputKeyDownListener(const FKeyEvent& InKeyEvent);

private:
	friend SSearchEverywhereWindow;
	uint32 LastKeyboardUser = 0;
	FKey LastKeyboardUserInput;
	TSharedPtr<FUICommandList> PluginCommands;
	TWeakPtr<SSearchEverywhereWindow> PluginWindow;
	FText PreviousSearchRequest;
	FDelegateHandle OnApplicationPreInputKeyDownListenerHandle;
	constexpr static int32 ResultChunkSize = 100; // todo change
	TSharedRef<FCallbackHandler, ESPMode::ThreadSafe> CallbackHandler = MakeShared<
		FCallbackHandler, ESPMode::ThreadSafe>(PluginWindow);
	TSharedRef<FSearcher> Searcher = MakeShared<FSearcher>(ResultChunkSize,
	                                                       CallbackHandler->GetMessageEndpoint());
};
