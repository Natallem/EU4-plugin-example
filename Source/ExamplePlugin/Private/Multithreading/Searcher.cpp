#include "Searcher.h"

#include "ResultItemFoundMsg.h"
#include "SettingsData/PropertyHolder.h"

FSearcher::FSearcher(int ChunkSize, const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe>& MessageEndpoint)
	: MessageEndpoint(MessageEndpoint),
	  ChunkSize(ChunkSize),
	  InputHandlerPtr(MakeShared<FInputHandler, ESPMode::ThreadSafe>(FString(), 0, 0)),
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
		TSharedPtr<FInputHandler, ESPMode::ThreadSafe> InputHandlerCopy;
		{
			FScopeLock ScopeLock(&CriticalSection);
			if (!InputHandlerPtr->bIsProcessRequestFinished)
			{
				if (const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> CurrentMessageEndpoint = MessageEndpoint.
					Pin())
				{
					FResultItemFoundMsg* Message = InputHandlerPtr->GetOutputFromBuffer();
					if (Message)
					{
						CurrentMessageEndpoint->Send(Message, CurrentMessageEndpoint->GetAddress());
					}
				}
				// if need to find more or need to fill buffer
				if (InputHandlerPtr->DesiredOutputSize > InputHandlerPtr->FoundOutputCounter || InputHandlerPtr->
					DesiredBufferSize < InputHandlerPtr->Buffer.Num() && !InputHandlerPtr->bIsAllDataFoundInBuffer)
				{
					InputHandlerCopy = InputHandlerPtr;
				}
			}
		}
		if (InputHandlerCopy.IsValid())
		{
			bIsInputChangedDuringSearching =
				ExecuteFindResultTask(InputHandlerCopy)
				|| FillBuffer(InputHandlerCopy);
		}
	}
	return 0;
}

void FSearcher::Stop()
{
	bShouldStopThread = true;
	{
		FScopeLock ScopeLock(&CriticalSection);
		InputHandlerPtr->bIsCancelled = true;
	}
	WakeUpWorkerEvent->Trigger();
}

void FSearcher::SetInput(const FString& NewInput)
{
	TWeakPtr<FInputHandler, ESPMode::ThreadSafe> InputHandlerWeakPtr;
	bool IsEmptyInput;
	{
		FScopeLock ScopeLock(&CriticalSection);
		InputHandlerPtr->bIsCancelled = true;
		InputHandlerPtr = MakeShared<FInputHandler, ESPMode::ThreadSafe>(NewInput, ChunkSize, ChunkSize);
		IsEmptyInput = InputHandlerPtr->bIsProcessRequestFinished;
		InputHandlerWeakPtr = InputHandlerPtr;
	}
	if (IsEmptyInput)
	{
		AllWordsFound(InputHandlerWeakPtr);
	}
	else
	{
		WakeUpWorkerEvent->Trigger();
	}
}


void FSearcher::FindMoreDataResult()
{
	{
		FScopeLock ScopeLock(&CriticalSection);
		InputHandlerPtr->DesiredOutputSize += ChunkSize;
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

bool FSearcher::ExecuteFindResultTask(TSharedPtr<FInputHandler, ESPMode::ThreadSafe>& InputTask)
{
	while (InputTask->DesiredOutputSize > InputTask->FoundOutputCounter)
	{
		TOptional<TSharedRef<ISearchableItem>> FoundWord = FPropertyHolder::Get().FindNextWord(InputTask);
		if (FoundWord)
		{
			if (!AddFoundItemToResult(MoveTemp(FoundWord.GetValue()), InputTask))
			{
				return false;
			}
		}
		else
		{
			// input changed -> skip waiting
			// finished searching -> notify main thread but do not need to skip waiting
			if (InputTask->bIsCancelled || !AllWordsFound(InputTask))
			{
				return false;
			}
			break;
		}
	}
	return true;
}

bool FSearcher::FillBuffer(TSharedPtr<FInputHandler, ESPMode::ThreadSafe>& Task) const
{
	if (!Task->bIsProcessRequestFinished)
	{
		while (Task->DesiredBufferSize > Task->Buffer.Num())
		{
			TOptional<TSharedRef<ISearchableItem>> FoundWord = FPropertyHolder::Get().FindNextWord(Task);
			if (FoundWord)
			{
				Task->Buffer.Push(MoveTemp(FoundWord.GetValue()));
			}
			else
			{
				return Task->bIsAllDataFoundInBuffer = !Task->bIsCancelled; // todo check if ok
			}
		}
	}
	return true;
}

bool FSearcher::AddFoundItemToResult(TSharedRef<ISearchableItem>&& Item,
                                     TSharedPtr<FInputHandler, ESPMode::ThreadSafe>& Task)
{
	FScopeLock ScopeLock(&CriticalSection);
	if (!Task->bIsCancelled)
	{
		++InputHandlerPtr->FoundOutputCounter;
		if (const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> CurrentMessageEndpoint = MessageEndpoint.Pin())
		{
			CurrentMessageEndpoint->Send(
				new FResultItemFoundMsg(false, MoveTemp(Item)), CurrentMessageEndpoint->GetAddress());
		}
		return true;
	}
	return false;
}

bool FSearcher::AllWordsFound(const TWeakPtr<FInputHandler, ESPMode::ThreadSafe> InputTaskWeakPtr)
{
	FScopeLock ScopeLock(&CriticalSection);
	if (const TSharedPtr<FInputHandler, ESPMode::ThreadSafe> InputTaskPtr = InputTaskWeakPtr.Pin())
	{
		if (!InputTaskPtr->bIsCancelled)
		{
			InputTaskPtr->bIsProcessRequestFinished = true;
			if (const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> CurrentMessageEndpoint = MessageEndpoint.Pin())
			{
				CurrentMessageEndpoint->Send(
					new FResultItemFoundMsg(), CurrentMessageEndpoint->GetAddress());
			}
			return true;
		}
	}
	return false;
}

//todo call stop when window close
