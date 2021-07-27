#pragma once
#include "MessageEndpoint.h"
#include "Dictionary/Dictionary.h"
#include "ResultProcessing/InputResult.h"

class FSearcher : public FRunnable
{
private:
	struct FHeartbeatRecipient
	{
		FHeartbeatRecipient(const TWeakPtr<FMessageEndpoint, ESPMode::ThreadSafe>& MessageEndpoint,
		                    const FMessageAddress& ConnectionAddress)
			: MessageEndpoint(MessageEndpoint),
			  ConnectionAddress(ConnectionAddress)
		{
		}

		TWeakPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint;
		FMessageAddress ConnectionAddress;
	};

public:
	explicit FSearcher(int ChunkSize, const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe>& MessageEndpoint,
	                   const FMessageAddress& RecipientAddress);

	virtual ~FSearcher() override = default;
	void EnsureCompletion();

	virtual bool Init() override;
	bool ExecuteFindResultTask(FSearchTask& Task);
	bool FillBuffer(FSearchTask& Task);

	virtual uint32 Run() override;
	virtual void Stop() override;

	TPair<bool, TArray<FString>> GetRequestData();
	void SetInput(const FString& NewInput);
	void FindMoreDataResult();
	void NotifyMainThread();

private:
	uint32 ChunkSize;
	FThreadSafeCounter RequestCounter;
	TWeakPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint;
	TUniquePtr<FRunnableThread> Thread;
	FEventRef WakeUpWorkerEvent;
	FThreadSafeBool m_Kill = false;
	FCriticalSection InputOperationSection;
	FInputResult Result;
	FDictionary Dictionary;
	FHeartbeatRecipient Recipient;
	bool IsNotifiedMainThread = false; // todo maybe threadsafe? No need because call under lock

	bool SaveTaskStateToResult(FSearchTask& Task);
	bool AddFoundWordToResult(FString&& Word, int32 TaskId);
	bool AllWordsFound(int32 InputId);
};
