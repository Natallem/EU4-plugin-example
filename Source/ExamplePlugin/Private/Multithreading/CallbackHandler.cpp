#include "CallbackHandler.h"

#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "UI/SearchEverywhereWindow.h"
#include "UI/SearchEverywhereWidget.h"

FCallbackHandler::FCallbackHandler(TWeakPtr<SSearchEverywhereWindow>& WindowToPassResult): WindowToPassResult(
	WindowToPassResult)
{
	MessageEndpoint = FMessageEndpoint::Builder(TEXT("LiveLinkMessageBusSource"))
		.Handling<FWordsFound>(this, &FCallbackHandler::HandleFoundWords);
}

TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> FCallbackHandler::GetMessageEndpoint() const
{
	return MessageEndpoint;
}

FCallbackHandler::~FCallbackHandler()
{
	FMessageEndpoint::SafeRelease(MessageEndpoint);
}

void FCallbackHandler::HandleFoundWords(const FWordsFound& Message,
                                        const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	TSharedPtr<SSearchEverywhereWindow> Window = WindowToPassResult.Pin();
	if (Window.IsValid())
	{
		Window->GetSearchEverywhereWidget()->UpdateShownResults();
	}
}
