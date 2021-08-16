#pragma once
#include "CoreMinimal.h"

template <typename RequiredType>
struct TInputHandler
{
	TInputHandler(int32 Id, const FString& Input, int32 DesiredOutputSize,
	              int32 DesiredBufferSize)
		: Id(Id),
		  InputRequest(Input),
		  DesiredOutputSize(DesiredOutputSize),
		  DesiredBufferSize(DesiredBufferSize),
		  bIsProcessRequestFinished(Input.IsEmpty())
	{
		OutputToGive.Reserve(DesiredOutputSize);
		Buffer.Reserve(DesiredBufferSize);
	}

	bool MoveFromBufferToMainResult()
	{
		if (DesiredOutputSize > FoundOutputCounter && Buffer.Num() != 0)
		{
			if (OutputToGive.Num() == 0 && FoundOutputCounter + Buffer.Num() < DesiredOutputSize)
			{
				OutputToGive = MoveTemp(Buffer);
				Buffer.Reset();
				FoundOutputCounter += OutputToGive.Num();
			}
			else
			{
				const int32 ElementsNumber = FMath::Min(DesiredOutputSize - FoundOutputCounter, Buffer.Num());
				for (int i = 0; i < ElementsNumber; ++i)
				{
					OutputToGive.Add(MoveTemp(Buffer[i]));
				}
				Buffer.RemoveAt(0, ElementsNumber, false);
				FoundOutputCounter += ElementsNumber;
			}
			return true;
		}
		return false;
	}

	int32 Id;
	FString InputRequest;
	int32 DesiredOutputSize;
	int32 DesiredBufferSize;
	/** True then all data found and main thead notified about this */
	bool bIsProcessRequestFinished;
	TArray<int> PArray;
	int32 NextIndexToCheck = 0;
	int32 FoundOutputCounter = 0;
	TArray<RequiredType> OutputToGive;
	TArray<RequiredType> Buffer;
};
