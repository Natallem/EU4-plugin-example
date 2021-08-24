#pragma once
#include "CoreMinimal.h"
#include "ResultItemFoundMsg.h"
template <typename RequiredType>
struct TInputHandler
{
	TInputHandler(const FString& InInputRequest, int32 DesiredOutputSize, int32 DesiredBufferSize)
		: InputRequest(InInputRequest),
		  DesiredOutputSize(DesiredOutputSize),
		  DesiredBufferSize(DesiredBufferSize),
		  bIsProcessRequestFinished(InInputRequest.IsEmpty())
	{
		Buffer.Reserve(DesiredBufferSize);
	}

	FResultItemFoundMsg*  GetOutputFromBuffer()
	{
		
		// FResultItemFoundMsg* ClearSubject = new FResultItemFoundMsg();
		if (DesiredOutputSize > FoundOutputCounter && Buffer.Num() != 0)
		{
			TArray<RequiredType> OutputFromBuffer;
			if (FoundOutputCounter + Buffer.Num() < DesiredOutputSize)
			{
				OutputFromBuffer = MoveTemp(Buffer);
				Buffer.Reset();
				FoundOutputCounter += OutputFromBuffer.Num();
				if (bIsAllDataFoundInBuffer)
				{
					bIsProcessRequestFinished = true;
				}
			}
			else
			{
				const int32 ElementsNumber = FMath::Min(DesiredOutputSize - FoundOutputCounter, Buffer.Num());
				for (int i = 0; i < ElementsNumber; ++i)
				{
					OutputFromBuffer.Add(MoveTemp(Buffer[i]));
				}
				Buffer.RemoveAt(0, ElementsNumber, false);
				FoundOutputCounter += ElementsNumber;
			}
			return new FResultItemFoundMsg(bIsProcessRequestFinished, MoveTemp(OutputFromBuffer));
		}
		return nullptr;
	}

	FString InputRequest;
	FThreadSafeBool bIsCancelled = false;
	int32 DesiredOutputSize;
	int32 DesiredBufferSize;
	
	/** True then all data found and main thead notified about this */
	bool bIsProcessRequestFinished;
	
	/** True if all data found and rest of data is in buffer. */
	bool bIsAllDataFoundInBuffer = false;
	
	int32 NextIndexToCheck = 0;
	int32 FoundOutputCounter = 0;
	TArray<RequiredType> Buffer;
};
