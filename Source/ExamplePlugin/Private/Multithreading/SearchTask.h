#pragma once

template <typename RequiredType>
class TSearchTask
{
public:
	int32 TaskId;
	FString RequestString;
	int32 DesiredResultSize;
	int32 DesiredBufferSize;
	uint64 NextIndexToCheck;
	bool bIsCompleteSearching = false;
	TArray<RequiredType> Buffer;

	TSearchTask(int32 TaskId, FString&& RequestString, int32 DesiredResultSize, int32 DesiredBufferSize,
	            uint64 NextIndexToCheck)
		: TaskId(TaskId),
		  RequestString(MoveTemp(RequestString)),
		  DesiredResultSize(DesiredResultSize),
		  DesiredBufferSize(DesiredBufferSize),
		  NextIndexToCheck(NextIndexToCheck)
	{
		Buffer.Reserve(DesiredBufferSize);
	}
};
