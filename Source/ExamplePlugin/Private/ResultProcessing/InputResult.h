#ifdef NOTSKIP

#pragma once

#include "Multithreading/Searcher.h"

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

#endif
