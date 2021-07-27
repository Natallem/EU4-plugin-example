#pragma once
#include "MessageEndpoint.h"
#include "Dictionary/Dictionary.h"
#include "ResultProcessing/InputResult.h"

class FSearcher : public FRunnable
{
public:
	explicit FSearcher(int ChunkSize, const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe>& MessageEndpoint);

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
	FEventRef WakeUpWorkerEvent;
	FThreadSafeBool m_Kill = false;
	FCriticalSection InputOperationSection;
	FInputResult Result;
	FDictionary Dictionary;
	bool IsNotifiedMainThread = false;
	TUniquePtr<FRunnableThread> Thread;

	bool SaveTaskStateToResult(FSearchTask& Task);
	bool AddFoundWordToResult(FString&& Word, int32 TaskId);
	bool AllWordsFound(int32 InputId);
};
