// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "InputCoreTypes.h"
#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "Multithreading/ItemFoundMsg.h"
#include "Multithreading/Searcher.h"

struct FKeyEvent;
class FToolBarBuilder;
class FMenuBuilder;
class FUICommandList;
class SSearchEverywhereWindow;
class SWindow;


class FExamplePluginModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	/** This system callback used to preload FPropertyHolder, because settings data depending on loaded modules, so we
	 *need to collect settings dat (in FPropertyConstructor) when most of modules is loaded
	 */
	void OnMainFrameLoaded(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow);

	/** This listener was made to get user-id that calls plugin by shortcut. It listens for all key press events and
	 * saves id of user, that pressed shortcut to call plugin window. In PluginButtonClicked by user id we get previous
	 * focused widget.
	 * @see SSearchEverywhereWindow::PreviousFocusedWidget
	 */
	void OnApplicationPreInputKeyDownListener(const FKeyEvent& InKeyEvent);

	void PluginButtonClicked();

	void HandleFoundWords(const FItemFoundMsg& InMessage,
	                      const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& InContext);

	TSharedPtr<FUICommandList> PluginCommands;

	/** Handle for OnApplicationPreInputKeyDownListener function */
	FDelegateHandle OnApplicationPreInputKeyDownListenerHandle;

	/** Index of last user, that pressed shortcut to open plugin window. Need to get previous focused window to create SSearchEverywhereWindow */
	uint32 LastKeyboardUserIndex = 0;
	TWeakPtr<SSearchEverywhereWindow> PluginWindow;

	/** Saved last user request of previous opened window */
	FText PreviousSearchRequest;

	constexpr static int32 ResultChunkSize = 100;
	TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint = FMessageEndpoint::Builder(
			TEXT("ItemFoundEndpoint"))
		.Handling<FItemFoundMsg>(this, &FExamplePluginModule::HandleFoundWords);
	TSharedRef<FSearcher> Searcher = MakeShared<FSearcher>(ResultChunkSize,
	                                                       MessageEndpoint);
	friend SSearchEverywhereWindow;
};
