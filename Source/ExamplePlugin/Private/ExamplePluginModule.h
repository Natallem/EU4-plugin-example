// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "InputCoreTypes.h"
#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "Multithreading/ResultItemFoundMsg.h"
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
	void HandleFoundWords(const FResultItemFoundMsg& InMessage,
	                      const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& InContext);

	TSharedPtr<FUICommandList> PluginCommands;
	TWeakPtr<SSearchEverywhereWindow> PluginWindow;
	FText PreviousSearchRequest;

	FDelegateHandle OnApplicationPreInputKeyDownListenerHandle;
	uint32 LastKeyboardUserIndex = 0;
	FKey LastKeyboardUserInput;

	constexpr static int32 ResultChunkSize = 100;
	TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint = FMessageEndpoint::Builder(
			TEXT("ResultItemFoundEndpoint"))
		.Handling<FResultItemFoundMsg>(this, &FExamplePluginModule::HandleFoundWords);
	TSharedRef<FSearcher> Searcher = MakeShared<FSearcher>(ResultChunkSize,
	                                                       MessageEndpoint);

	friend SSearchEverywhereWindow;
};
