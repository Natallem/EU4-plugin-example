#pragma once
// #include "Messages/WordsFoundMessage.h"

class FCallbackHandler
{
public:
	explicit FCallbackHandler(TWeakPtr<class SSearchEverywhereWindow>& WindowToPassResult);
	TSharedPtr<class FMessageEndpoint, ESPMode::ThreadSafe> GetMessageEndpoint() const;
	~FCallbackHandler();

private:
	TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint;
	TWeakPtr<SSearchEverywhereWindow>& WindowToPassResult;
	void HandleFoundWords(const struct FWordsFound& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context);
};
