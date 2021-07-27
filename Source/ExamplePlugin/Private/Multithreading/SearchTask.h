#pragma once

class FSearchTask
{
public:
	int32 TaskId;
	FString Request;
	int32 DesiredResultSize;
	int32 DesiredBufferSize;
	TArray<int> PArray;
	uint64 NextIndexToCheck;
	bool bIsCompleteSearching = false;
	TArray<FString> Buffer;

	FSearchTask(int32 TaskId, FString&& Request, int32 DesiredResultSize, int32 DesiredBufferSize,
	            TArray<int>&& PArray, uint64 NextIndexToCheck)
		: TaskId(TaskId),
		  Request(MoveTemp(Request)),
		  DesiredResultSize(DesiredResultSize),
		  DesiredBufferSize(DesiredBufferSize),
		  PArray(MoveTemp(PArray)),
		  NextIndexToCheck(NextIndexToCheck)
	{
	}
};
