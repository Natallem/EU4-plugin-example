#pragma once

class FTask
{
public:
	int32 TaskId;
	FString Request;
	int32 DesiredSize;
	TArray<int> PArray;
	uint64 NextIndexToCheck;
	bool bIsCompleteSearching = false;

	FTask(int32 TaskId, FString&& Request, int32 DesiredSize, const TArray<int>& PArray, uint64 NextIndexToCheck)
		: TaskId(TaskId),
		  Request(MoveTemp(Request)),
		  DesiredSize(DesiredSize),
		  PArray(PArray),
		  NextIndexToCheck(NextIndexToCheck)
	{
	}
};
