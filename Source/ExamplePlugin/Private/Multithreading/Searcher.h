#pragma once
#include "MessageEndpoint.h"
#include "SearchTask.h"
#include "SettingsData/PropertyHolder.h"
#include "Configuration.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"


template <typename T>
class TSearchTask;

class FRunnableThread;

template <typename RequiredType>
struct TInputResult
{
	TInputResult(int32 Id, const FString& Input, int32 DesiredResultSize, int32 DesiredBufferSize);

	void MoveFromBufferToMainResult(class FSearcher& Searcher);

	int32 Id;
	FString Input;
	int32 DesiredResultSize;
	int32 DesiredBufferSize;
	bool IsFinishedProcess;
	TArray<int> PArray;
	int32 NextIndexToCheck = 0;
	int32 FoundResultCounter = 0;
	TArray<RequiredType> ResultToGive;
	TArray<RequiredType> Buffer;
};

class FSearcher : public FRunnable
{
public:
	using FSearchTask = TSearchTask<RequiredType>;
	using FInputResult = TInputResult<RequiredType>;

	explicit FSearcher(int ChunkSize, const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe>& MessageEndpoint);

	virtual ~FSearcher() override = default;
	void EnsureCompletion();

	virtual bool Init() override;
	bool ExecuteFindResultTask(FSearchTask& Task);
	bool FillBuffer(FSearchTask& Task);

	virtual uint32 Run() override;
	virtual void Stop() override;

	TPair<bool, TArray<RequiredType>> GetRequestData();
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
	FPropertyHolder& PropertyHolder;
	bool IsNotifiedMainThread = false;
	TUniquePtr<FRunnableThread> Thread;

	bool SaveTaskStateToResult(FSearchTask& Task);
	bool AddFoundItemToResult(RequiredType&& Word, int32 TaskId);
	bool AllWordsFound(int32 InputId);
};

template <typename RequiredType>
TInputResult<RequiredType>::TInputResult(int32 Id, const FString& Input, int32 DesiredResultSize,
                                         int32 DesiredBufferSize)
	: Id(Id),
	  Input(Input),
	  DesiredResultSize(DesiredResultSize),
	  DesiredBufferSize(DesiredBufferSize),
	  IsFinishedProcess(Input.IsEmpty())
{
}

/*
*Called with mutex
*/
template <typename RequiredType>
void TInputResult<RequiredType>::MoveFromBufferToMainResult(FSearcher& Searcher)
{
	if (DesiredResultSize > FoundResultCounter)
	{
		if (Buffer.Num() != 0)
		{
			if (ResultToGive.Num() == 0 && FoundResultCounter + Buffer.Num() < DesiredResultSize)
			{
				ResultToGive = MoveTemp(Buffer);
				Buffer.Reset();
				FoundResultCounter += ResultToGive.Num();
				Searcher.NotifyMainThread();
			}
			else
			{
				const int32 ElementsNumber = FMath::Min(DesiredResultSize - FoundResultCounter, Buffer.Num());
				for (int i = 0; i < ElementsNumber; ++i)
				{
					ResultToGive.Add(MoveTemp(Buffer[i]));
				}
				Buffer.RemoveAt(0, ElementsNumber, false);
				FoundResultCounter += ElementsNumber;
				Searcher.NotifyMainThread();
			}
		}
	}
}
