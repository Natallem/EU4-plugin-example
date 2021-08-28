#include "Searcher.h"

#include "ItemFoundMsg.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "SettingsData/PropertyHolder.h"
#include "SettingsData/SettingItemTypes.h"

FSearcher::FSearcher(int ChunkSize, const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe>& MessageEndpoint)
	: MessageEndpoint(MessageEndpoint),
	  ChunkSize(ChunkSize),
	  InputData(MakeShared<FInputData, ESPMode::ThreadSafe>(FString(), 0, 0, All)),
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
		if (TSharedPtr<FInputData, ESPMode::ThreadSafe> InputHandlerCopy = CheckIfCurrentInputNeedToBeProcessed())
		{
			bIsInputChangedDuringSearching =
				ProcessCurrentInput(InputHandlerCopy)
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
		InputData->bIsCancelled = true;
	}
	WakeUpWorkerEvent->Trigger();
}

void FSearcher::SetInput(const FString& NewInput, ESettingType SearchType)
{
	TWeakPtr<FInputData, ESPMode::ThreadSafe> InputHandlerWeakPtr;
	bool IsEmptyInput;
	{
		FScopeLock ScopeLock(&CriticalSection);
		// cancel previous request and replace processing input data with new input data
		InputData->bIsCancelled = true;
		InputData = MakeShared<FInputData, ESPMode::ThreadSafe>(NewInput, ChunkSize, ChunkSize, SearchType);
		IsEmptyInput = InputData->bIsProcessingFinished;
		InputHandlerWeakPtr = InputData;
	}
	if (IsEmptyInput)
	{
		AllWordsFound(InputHandlerWeakPtr);
	}
	else
	{
		// trigger search thread if it was waiting for new input
		WakeUpWorkerEvent->Trigger();
	}
}


void FSearcher::FindMoreDataResult()
{
	{
		FScopeLock ScopeLock(&CriticalSection);
		InputData->DesiredOutputSize += ChunkSize;
	}
	WakeUpWorkerEvent->Trigger();
}

TSharedPtr<FInputData, ESPMode::ThreadSafe> FSearcher::CheckIfCurrentInputNeedToBeProcessed()
{
	FScopeLock ScopeLock(&CriticalSection);
	if (!InputData->bIsProcessingFinished)
	{
		InputData->MoveBufferedDataToOutput(MessageEndpoint);
		// if need to find more or need to fill buffer
		if (InputData->DesiredOutputSize > InputData->FoundOutputCounter || InputData->
			DesiredBufferSize < InputData->Buffer.Num() && !InputData->bIsAllDataFoundInBuffer)
		{
			return InputData;
		}
	}
	return nullptr;
}

void FSearcher::EnsureCompletion()
{
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
	}
}

bool FSearcher::ProcessCurrentInput(TSharedPtr<FInputData, ESPMode::ThreadSafe>& InputTask)
{
	while (InputTask->DesiredOutputSize > InputTask->FoundOutputCounter)
	{
		TOptional<TSharedRef<ISearchableItem>> FoundWord = FPropertyHolder::Get().FindNextItem(InputTask);
		if (FoundWord)
		{
			if (!SendFoundItem(MoveTemp(FoundWord.GetValue()), InputTask))
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

bool FSearcher::FillBuffer(TSharedPtr<FInputData, ESPMode::ThreadSafe>& InputData)
{
	if (!InputData->bIsProcessingFinished)
	{
		while (InputData->DesiredBufferSize > InputData->Buffer.Num())
		{
			TOptional<TSharedRef<ISearchableItem>> FoundWord = FPropertyHolder::Get().FindNextItem(InputData);
			if (FoundWord)
			{
				InputData->Buffer.Push(MoveTemp(FoundWord.GetValue()));
			}
			else
			{
				return InputData->bIsAllDataFoundInBuffer = !InputData->bIsCancelled;
			}
		}
	}
	return true;
}

bool FSearcher::SendFoundItem(TSharedRef<ISearchableItem>&& Item,
                              TSharedPtr<FInputData, ESPMode::ThreadSafe>& InputDataWeakPtr)
{
	FScopeLock ScopeLock(&CriticalSection);
	if (!InputDataWeakPtr->bIsCancelled)
	{
		++InputData->FoundOutputCounter;
		if (const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> CurrentMessageEndpoint = MessageEndpoint.Pin())
		{
			CurrentMessageEndpoint->Send(
				new FItemFoundMsg(false, MoveTemp(Item)), CurrentMessageEndpoint->GetAddress());
			return true;
		}
	}
	return false;
}

bool FSearcher::AllWordsFound(const TWeakPtr<FInputData, ESPMode::ThreadSafe> InputTaskWeakPtr)
{
	FScopeLock ScopeLock(&CriticalSection);
	if (const TSharedPtr<FInputData, ESPMode::ThreadSafe> InputTaskPtr = InputTaskWeakPtr.Pin())
	{
		if (!InputTaskPtr->bIsCancelled)
		{
			InputTaskPtr->bIsProcessingFinished = true;
			if (const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> CurrentMessageEndpoint = MessageEndpoint.Pin())
			{
				CurrentMessageEndpoint->Send(
					new FItemFoundMsg(), CurrentMessageEndpoint->GetAddress());
				return true;
			}
		}
	}
	return false;
}
