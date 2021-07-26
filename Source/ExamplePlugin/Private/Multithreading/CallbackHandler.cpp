#include "CallbackHandler.h"

#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "HAL/ThreadManager.h"
#include "UI/SearchEverywhereWindow.h"

FCallbackHandler::FCallbackHandler(TWeakPtr<SSearchEverywhereWindow>& WindowToPassResult): WindowToPassResult(
	WindowToPassResult)
{
	MessageEndpoint = FMessageEndpoint::Builder(TEXT("LiveLinkMessageBusSource"))
	                  .Handling<FWordsFound>(this, &FCallbackHandler::HandleFoundWords)
	                  // .ReceivingOnThread(ENamedThreads::AnyThread)
	;

	// .ReceivingOnAnyThread();
	uint32 ThreadId = FPlatformTLS::GetCurrentThreadId();
	FString ThreadName = FThreadManager::Get().GetThreadName(ThreadId);
	// FThreadManager::Get().
	// MessageEndpoint->SetRecipientThread(ENamedThreads::);
}

TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> FCallbackHandler::GetMessageEndpoint() const
{
	return MessageEndpoint;
}

void FCallbackHandler::HandleFoundWords(const FWordsFound& Message,
                                        const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	TSharedPtr<SSearchEverywhereWindow> Window = WindowToPassResult.Pin();
	if (Window.IsValid())
	{
		Window->UpdateShownResults();
	}
}
































