#include "InputData.h"

#include "ItemFoundMsg.h"

FInputData::FInputData(const FString& InInputRequest, int32 DesiredOutputSize, int32 DesiredBufferSize,
                       ESettingType SearchType): InputRequest(InInputRequest),
                                                 DesiredOutputSize(DesiredOutputSize),
                                                 DesiredBufferSize(DesiredBufferSize),
                                                 SearchType(SearchType),
                                                 bIsProcessingFinished(InInputRequest.IsEmpty())
{
	Buffer.Reserve(DesiredBufferSize);
}

void FInputData::MoveBufferedDataToOutput(const TWeakPtr<FMessageEndpoint, ESPMode::ThreadSafe>& WeakMessageEndpoint)
{
	if (DesiredOutputSize > FoundOutputCounter && Buffer.Num() != 0)
	{
		TArray<TSharedRef<ISearchableItem>> OutputFromBuffer;
		if (FoundOutputCounter + Buffer.Num() < DesiredOutputSize)
		{
			OutputFromBuffer = MoveTemp(Buffer);
			Buffer.Reset();
			FoundOutputCounter += OutputFromBuffer.Num();
			if (bIsAllDataFoundInBuffer)
			{
				bIsProcessingFinished = true;
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
		if (const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint = WeakMessageEndpoint.Pin())
		{
			MessageEndpoint->Send(new FItemFoundMsg(bIsProcessingFinished,
			                                        MoveTemp(OutputFromBuffer)),
			                      MessageEndpoint->GetAddress());
		}
	}
}
