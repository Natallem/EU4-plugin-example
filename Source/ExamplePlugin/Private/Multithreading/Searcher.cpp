#include "Searcher.h"

#include "Task.h"
#include "Messages/WordsFoundMessage.h"

FSearcher::FSearcher(int ChunkSize,
                     const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe>& MessageEndpoint,
                     const FMessageAddress& RecipientAddress)
	: ChunkSize(ChunkSize),
	  Thread(FRunnableThread::Create(this, TEXT("SearchEverywhereThread"), 0, TPri_Normal)),
	  Result(0, FString(), 0),
	  Recipient(MessageEndpoint, RecipientAddress)
{
}

bool FSearcher::Init()
{
	return true;
}

bool FSearcher::AddFoundWordToResult(FString&& Word, int32 TaskId)
{
	FScopeLock ScopeLock(&InputOperationSection);
	if (TaskId == RequestCounter.GetValue())
	{
		Result.ResultToGive.Add(MoveTemp(Word));
		++Result.FoundResultCounter;
		NotifyMainThread();
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
		if (TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> CurrentMessageEndpoint = Recipient.MessageEndpoint.Pin())
		{
			CurrentMessageEndpoint->Send(new FWordsFound(), Recipient.ConnectionAddress);
		}
	}
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

bool FSearcher::SaveTaskStateToResult(FTask& Task)
{
	FScopeLock ScopeLock(&InputOperationSection);
	if (Task.TaskId == RequestCounter.GetValue())
	{
		ensureAlways(Result.Id == Task.TaskId);
		Result.Input = MoveTemp((Task.Request));
		Result.PArray = MoveTemp(Task.PArray);
		Result.NextIndexToCheck = Task.NextIndexToCheck;
		return true;
	}
	return false;
}

uint32 FSearcher::Run()
{
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);
	UE_LOG(LogTemp, Log, TEXT("EP : FSearcher is Running!"));
	bool bIsInputChangedDuringSearhing = false;
	while (!m_Kill)
	{
		if (!bIsInputChangedDuringSearhing)
		{
			UE_LOG(LogTemp, Log, TEXT("EP : FSearcher waiting"));
			WakeUpWorkerEvent->Wait();
		}
		else
		{
			bIsInputChangedDuringSearhing = false;
		}
		if (m_Kill)
		{
			return 0;
		}
		TOptional<FTask> Task;
		{
			FScopeLock ScopeLock(&InputOperationSection);
			if (!Result.IsFinishedProcess && Result.DesiredResultSize > Result.FoundResultCounter)
			{
				Task = FTask(RequestCounter.GetValue(), MoveTemp(Result.Input),
				             Result.DesiredResultSize - Result.FoundResultCounter, Result.PArray,
				             Result.NextIndexToCheck);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("EP : FSearcher got a task"));
		if (Task.IsSet())
		{
			if (Task->PArray.Num() == 0)
			{
				Task->PArray = FDictionary::CreatePArray(Task->Request);
				UE_LOG(LogTemp, Log, TEXT("EP : FSearcher calculated p-array"));
			}
			while (Task->DesiredSize > 0)
			{
				TOptional<FString> FoundWord = Dictionary.FindNextWord(Task.GetValue(), RequestCounter);

				if (FoundWord)
				{
					if (!AddFoundWordToResult(MoveTemp(FoundWord.GetValue()), Task->TaskId))
					{
						UE_LOG(LogTemp, Log, TEXT("EP : FSearcher couldn't add a found word"));
						bIsInputChangedDuringSearhing = true;
						break;
					}
					else
					{
						UE_LOG(LogTemp, Log, TEXT("EP : FSearcher added a found word"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("EP : FSearcher not found word"));

					// input changed -> skip waiting
					// finished searching -> notify main thread but do not need to skip waiting
					if (!Task->bIsCompleteSearching || Task->bIsCompleteSearching && !AllWordsFound(Task->TaskId))
					{
						bIsInputChangedDuringSearhing = true;
					}
					break;
				}
			}
			if (!bIsInputChangedDuringSearhing)
			{
				bIsInputChangedDuringSearhing = !SaveTaskStateToResult(Task.GetValue());
			}
		}

		// FPlatformProcess::Sleep(1);
		// UE_LOG(LogTemp, Log, TEXT("EP : FSearcher says hello"));
		// OnNewDataFound().ExecuteIfBound();
	}
	return 0;
}

void FSearcher::Stop()
{
	m_Kill = true;
	RequestCounter.Increment();
	WakeUpWorkerEvent->Trigger();
}

TArray<FString> FSearcher::GetRequestData()
{
	TArray<FString> ReturnResult;
	{
		UE_LOG(LogTemp, Log, TEXT("EP : FSearcher returning found result"));
		FScopeLock ScopeLock(&InputOperationSection);
		ensureAlways(RequestCounter.GetValue() == Result.Id);
		ReturnResult = MoveTemp(Result.ResultToGive);
		Result.ResultToGive.Empty();
		IsNotifiedMainThread = false;
	}
	return ReturnResult;
}

void FSearcher::EnsureCompletion()
{
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
	}
}
/*
FSearcher::FDataPortionFoundDelegate& FSearcher::OnNewDataFound()
{
	return DataPortionFoundDelegate;
}
*/

bool FSearcher::ReturnBoolFunc()
{
	return true;
}

void FSearcher::SetInput(const FString& NewInput)
{
	RequestCounter.Increment();
	bool IsEmptyInput;
	int32 Id;
	{
		FScopeLock ScopeLock(&InputOperationSection);
		Result = FInputResult(RequestCounter.GetValue(), NewInput, ChunkSize);
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
