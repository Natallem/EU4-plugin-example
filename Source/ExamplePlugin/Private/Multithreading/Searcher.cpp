#include "Searcher.h"

#include "SearchTask.h"
#include "ResultItemFoundMsg.h"

FSearcher::FSearcher(int ChunkSize, const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe>& MessageEndpoint)
	: MessageEndpoint(MessageEndpoint),
	  ChunkSize(ChunkSize),
	  InputHandler(0, FString(), 0, 0),
	  Thread(FRunnableThread::Create(this, TEXT("SearchEverywhereThread"), 0, TPri_Normal))
{
}

bool FSearcher::Init()
{
	return true;
}

uint32 FSearcher::Run()
{
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);
	bool bIsInputChangedDuringSearching = false;
	while (!bShouldStopThread)
	{
		if (!bIsInputChangedDuringSearching)
		{
			WakeUpWorkerEvent->Wait();
		}
		else
		{
			bIsInputChangedDuringSearching = false;
		}
		if (bShouldStopThread)
		{
			return 0;
		}
		TOptional<FSearchTask> FindResultTask;
		{
			FScopeLock ScopeLock(&InputOperationSection);
			if (!InputHandler.bIsProcessRequestFinished)
			{
				if (InputHandler.MoveFromBufferToMainResult())
				{
					NotifyMainThread();
				};
				// if need to find more or need to fill buffer
				if (InputHandler.DesiredOutputSize > InputHandler.FoundOutputCounter)
				{
					FindResultTask =
						FSearchTask(RequestCounter.GetValue(),
						            MoveTemp(InputHandler.InputRequest),
						            InputHandler.DesiredOutputSize - InputHandler.FoundOutputCounter,
						            InputHandler.DesiredBufferSize - InputHandler.Buffer.Num(),
						            InputHandler.NextIndexToCheck);
				}
			}
		}
		if (FindResultTask.IsSet())
		{
			bIsInputChangedDuringSearching =
				ExecuteFindResultTask(FindResultTask.GetValue())
				|| FillBuffer(FindResultTask.GetValue())
				|| !SaveTaskStateToResult(FindResultTask.GetValue());
		}
	}
	return 0;
}

void FSearcher::Stop()
{
	bShouldStopThread = true;
	RequestCounter.Increment();
	WakeUpWorkerEvent->Trigger();
}

void FSearcher::SetInput(const FString& NewInput)
{
	RequestCounter.Increment();
	bool IsEmptyInput;
	int32 Id;
	{
		FScopeLock ScopeLock(&InputOperationSection);
		InputHandler = FInputHandler(RequestCounter.GetValue(), NewInput, ChunkSize, ChunkSize);
		IsEmptyInput = InputHandler.bIsProcessRequestFinished;
		Id = InputHandler.Id;
	}
	if (IsEmptyInput)
	{
		AllWordsFound(Id);
	}
	else
	{
		WakeUpWorkerEvent->Trigger();
	}
}

TPair<bool, TArray<RequiredType>> FSearcher::GetRequestData()
{
	TArray<RequiredType> ReturnResult;
	bool bIsSearchingFinished;
	{
		FScopeLock ScopeLock(&InputOperationSection);
		ensureAlways(RequestCounter.GetValue() == InputHandler.Id);
		ReturnResult = MoveTemp(InputHandler.OutputToGive);
		bIsSearchingFinished = InputHandler.bIsProcessRequestFinished;
		InputHandler.OutputToGive.Reset();
		bIsNotifiedMainThread = false;
	}
	return TPair<bool, TArray<RequiredType>>{bIsSearchingFinished, ReturnResult};
}

void FSearcher::FindMoreDataResult()
{
	{
		FScopeLock ScopeLock(&InputOperationSection);
		ensureAlways(InputHandler.Id == RequestCounter.GetValue());
		InputHandler.DesiredOutputSize += ChunkSize;
	}
	WakeUpWorkerEvent->Trigger();
}

void FSearcher::EnsureCompletion()
{
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
	}
}

bool FSearcher::ExecuteFindResultTask(FSearchTask& FindResultTask)
{
	while (FindResultTask.DesiredResultSize > 0)
	{
		TOptional<uint64> FoundWord = FPropertyHolder::Get().FindNextWord(FindResultTask, RequestCounter);
		if (FoundWord)
		{
			--FindResultTask.DesiredResultSize;
			if (!AddFoundItemToResult(MoveTemp(FoundWord.GetValue()), FindResultTask.TaskId))
			{
				return true;
			}
		}
		else
		{
			// input changed -> skip waiting
			// finished searching -> notify main thread but do not need to skip waiting
			if (!FindResultTask.bIsCompleteSearching || FindResultTask.bIsCompleteSearching && !AllWordsFound(
				FindResultTask.TaskId))
			{
				return true;
			}
			break;
		}
	}
	return false;
}

bool FSearcher::FillBuffer(FSearchTask& Task) const
{
	if (!Task.bIsCompleteSearching)
	{
		while (Task.DesiredBufferSize > 0)
		{
			TOptional<RequiredType> FoundWord = FPropertyHolder::Get().FindNextWord(Task, RequestCounter);
			if (FoundWord)
			{
				--Task.DesiredBufferSize;
				Task.Buffer.Push(MoveTemp(FoundWord.GetValue()));
			}
			else
			{
				return !Task.bIsCompleteSearching;
			}
		}
	}
	return false;
}

bool FSearcher::SaveTaskStateToResult(FSearchTask& Task)
{
	FScopeLock ScopeLock(&InputOperationSection);
	if (Task.TaskId == RequestCounter.GetValue())
	{
		ensureAlways(InputHandler.Id == Task.TaskId);
		InputHandler.InputRequest = MoveTemp((Task.RequestString));
		InputHandler.NextIndexToCheck = Task.NextIndexToCheck;
		if (Task.bIsCompleteSearching && Task.Buffer.Num() == 0)
		{
			InputHandler.bIsProcessRequestFinished = true;
		}
		if (Task.Buffer.Num() != 0)
		{
			if (InputHandler.Buffer.Num() == 0)
			{
				InputHandler.Buffer = MoveTemp(Task.Buffer);
			}
			else
			{
				InputHandler.Buffer.Append(MoveTemp(Task.Buffer));
			}
		}
		return true;
	}
	return false;
}

bool FSearcher::AddFoundItemToResult(RequiredType&& Item, int32 TaskId)
{
	FScopeLock ScopeLock(&InputOperationSection);
	if (TaskId == RequestCounter.GetValue() && InputHandler.Id == TaskId)
	{
		InputHandler.OutputToGive.Add(MoveTemp(Item));
		++InputHandler.FoundOutputCounter;
		NotifyMainThread();
		return true;
	}
	return false;
}

bool FSearcher::AllWordsFound(int32 InputId)
{
	FScopeLock ScopeLock(&InputOperationSection);
	if (InputId == RequestCounter.GetValue())
	{
		InputHandler.bIsProcessRequestFinished = true;
		NotifyMainThread();
		return true;
	}
	return false;
}

void FSearcher::NotifyMainThread()
{
	if (!bIsNotifiedMainThread)
	{
		bIsNotifiedMainThread = true;
		if (const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> CurrentMessageEndpoint = MessageEndpoint.Pin())
		{
			CurrentMessageEndpoint->Send(new FResultItemFoundMsg(), CurrentMessageEndpoint->GetAddress());
		}
	}
}
