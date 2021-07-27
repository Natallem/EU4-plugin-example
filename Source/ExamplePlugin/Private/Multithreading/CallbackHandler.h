#pragma once
#include "Messages/WordsFoundMessage.h"

class FCallbackHandler
{
public:
	TSharedPtr<class FMessageEndpoint, ESPMode::ThreadSafe> GetMessageEndpoint() const;

	explicit FCallbackHandler(TWeakPtr<class SSearchEverywhereWindow>& WindowToPassResult);

private:
	TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint;
	TWeakPtr<SSearchEverywhereWindow>& WindowToPassResult;
	void HandleFoundWords(const FWordsFound& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context);
};
