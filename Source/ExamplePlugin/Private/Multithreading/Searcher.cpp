#include "Searcher.h"

#include "SearchTask.h"
#include "Messages/WordsFoundMessage.h"

//todo think about reserve for array
FSearcher::FSearcher(int ChunkSize, const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe>& MessageEndpoint)
	: ChunkSize(ChunkSize),
	  MessageEndpoint(MessageEndpoint),
	  Result(0, FString(), 0, 0),
	  PropertyHolder(FPropertyHolder::Get()),
	  Thread(FRunnableThread::Create(this, TEXT("SearchEverywhereThread"), 0, TPri_Normal))
{
}

bool FSearcher::Init()
{
	return true;
}


void FSearcher::EnsureCompletion()
{
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
	}
}

void FSearcher::Stop()
{
	m_Kill = true;
	RequestCounter.Increment();
	WakeUpWorkerEvent->Trigger();
}


uint32 FSearcher::Run()
{
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);
	UE_LOG(LogTemp, Log, TEXT("EP : FSearcher is Running!"));
	bool bIsInputChangedDuringSearching = false;
	while (!m_Kill)
	{
		if (!bIsInputChangedDuringSearching)
		{
			UE_LOG(LogTemp, Log, TEXT("EP : FSearcher waiting"));
			WakeUpWorkerEvent->Wait(); //todo need to check before in need to find more answers 
		}
		else
		{
			bIsInputChangedDuringSearching = false;
		}
		if (m_Kill)
		{
			return 0;
		}
		TOptional<FSearchTask> FindResultTask;
		TOptional<FSearchTask> FillBufferTask;
		{
			FScopeLock ScopeLock(&InputOperationSection);
			if (!Result.IsFinishedProcess)
			{
				Result.MoveFromBufferToMainResult(*this);
				// if need to find more or need to fill buffer
				if (Result.DesiredResultSize > Result.FoundResultCounter)
				{
					FindResultTask =
						FSearchTask(RequestCounter.GetValue(),
						            MoveTemp(Result.Input),
						            Result.DesiredResultSize - Result.FoundResultCounter,
						            Result.DesiredBufferSize - Result.Buffer.Num(),
						            MoveTemp(Result.PArray),
						            Result.NextIndexToCheck);
				}
			}
		}
		UE_LOG(LogTemp, Log, TEXT("EP : FSearcher got a task"));
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

bool FSearcher::ExecuteFindResultTask(FSearchTask& FindResultTask)
{
	if (FindResultTask.PArray.Num() == 0)
	{
		FindResultTask.PArray = FPropertyHolder::CreatePArray(FindResultTask.Request);
		UE_LOG(LogTemp, Log, TEXT("EP : FSearcher calculated p-array"));
	}
	while (FindResultTask.DesiredResultSize > 0)
	{
		TOptional<uint64> FoundWord = PropertyHolder.FindNextWord(FindResultTask, RequestCounter);
		if (FoundWord)
		{
			--FindResultTask.DesiredResultSize;
			if (!AddFoundItemToResult(MoveTemp(FoundWord.GetValue()), FindResultTask.TaskId))
			{
				UE_LOG(LogTemp, Log, TEXT("EP : FSearcher couldn't add a found word"));
				return true;
			}

			UE_LOG(LogTemp, Log, TEXT("EP : FSearcher added a found word"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("EP : FSearcher not found word"));

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

/**
 *Return true if input change while processing
 */
bool FSearcher::FillBuffer(FSearchTask& Task)
{
	if (!Task.bIsCompleteSearching)
	{
		while (Task.DesiredBufferSize > 0)
		{
			TOptional<RequiredType> FoundWord = PropertyHolder.FindNextWord(Task, RequestCounter);
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

bool FSearcher::AddFoundItemToResult(RequiredType&& Item, int32 TaskId)
// todo maybe add not by one word? Maybe use thread safe queue?
{
	FScopeLock ScopeLock(&InputOperationSection);
	if (TaskId == RequestCounter.GetValue())
	{
		Result.ResultToGive.Add(MoveTemp(Item));
		++Result.FoundResultCounter;
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
		Result.IsFinishedProcess = true;
		NotifyMainThread();
		return true;
	}
	return false;
}

/**
 *Return if successfully saved
 */
bool FSearcher::SaveTaskStateToResult(FSearchTask& Task)
{
	FScopeLock ScopeLock(&InputOperationSection);
	if (Task.TaskId == RequestCounter.GetValue())
	{
		ensureAlways(Result.Id == Task.TaskId);
		Result.Input = MoveTemp((Task.Request));
		Result.PArray = MoveTemp(Task.PArray);
		Result.NextIndexToCheck = Task.NextIndexToCheck;
		if (Task.bIsCompleteSearching && Task.Buffer.Num() != 0)
		{
			Result.IsFinishedProcess = true;
		}
		if (Task.Buffer.Num() != 0)
		{
			if (Result.Buffer.Num() == 0)
			{
				Result.Buffer = MoveTemp(Task.Buffer);
			}
			else
			{
				/*for (int i = 0; i < Task.Buffer.Num(); ++i)
				{
					Result.Buffer.Add(MoveTemp(Task.Buffer[i]));
				}*/
				// Task.Buffer.Reset();// todo maybe no need to reset?
				//todo check that move operator call
				Result.Buffer.Append(MoveTemp(Task.Buffer));
			}
		}
		return true;
	}
	return false;
}

/** Called only with Lock
 *
 */
void FSearcher::NotifyMainThread()
{
	if (!IsNotifiedMainThread)
	{
		IsNotifiedMainThread = true;
		if (const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> CurrentMessageEndpoint = MessageEndpoint.Pin())
		{
			CurrentMessageEndpoint->Send(new FWordsFound(), CurrentMessageEndpoint->GetAddress());
		}
	}
}

TPair<bool, TArray<RequiredType>> FSearcher::GetRequestData()
{
	TArray<RequiredType> ReturnResult;
	bool bIsSearchingFinished;
	{
		UE_LOG(LogTemp, Log, TEXT("EP : FSearcher returning found result"));
		FScopeLock ScopeLock(&InputOperationSection);
		ensureAlways(RequestCounter.GetValue() == Result.Id);
		ReturnResult = MoveTemp(Result.ResultToGive);
		bIsSearchingFinished = Result.IsFinishedProcess;
		Result.ResultToGive.Reset();
		IsNotifiedMainThread = false;
	}
	return TPair<bool, TArray<RequiredType>>{bIsSearchingFinished, ReturnResult};
}

void FSearcher::SetInput(const FString& NewInput)
{
	RequestCounter.Increment();
	bool IsEmptyInput;
	int32 Id;
	{
		FScopeLock ScopeLock(&InputOperationSection);
		Result = FInputResult(RequestCounter.GetValue(), NewInput, ChunkSize, ChunkSize);
		IsEmptyInput = Result.IsFinishedProcess;
		Id = Result.Id;
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

void FSearcher::FindMoreDataResult()
{
	{
		FScopeLock ScopeLock(&InputOperationSection);
		ensureAlways(Result.Id == RequestCounter.GetValue());
		Result.DesiredResultSize += ChunkSize;
	}
	WakeUpWorkerEvent->Trigger();
}
