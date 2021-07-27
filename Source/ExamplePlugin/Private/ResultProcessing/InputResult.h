#pragma once
// #include "Multithreading/Searcher.h"

class FInputResult
{
public:
	FInputResult(int32 Id, const FString& Input, int32 DesiredResultSize, int32 DesiredBufferSize);
	void MoveFromBufferToMainResult(class FSearcher& Searcher);

	int32 Id;
	FString Input;
	int32 DesiredResultSize;
	int32 DesiredBufferSize;
	bool IsFinishedProcess;
	TArray<int> PArray;
	int32 NextIndexToCheck = 0;
	int32 FoundResultCounter = 0;
	TArray<FString> ResultToGive;
	TArray<FString> Buffer;
};
