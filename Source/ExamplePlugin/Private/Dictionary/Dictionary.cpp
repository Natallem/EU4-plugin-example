#ifdef NOTSKIP
#include "Dictionary.h"

#include "Multithreading/SearchTask.h"
#include "Misc/FileHelper.h"

FDictionary::FDictionary()
{
	GetTxtData(DictionaryPath);
}

void FDictionary::GetTxtData(const FString& Path)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*Path))
	{
		UE_LOG(LogTemp, Warning, TEXT("File for dictionary not found : '%s'"), *Path);
		return;
	}
	FFileHelper::LoadANSITextFileToStrings(*Path, NULL, DictionaryStringArray);
}

TArray<int> FDictionary::CreatePArray(const FString& Pattern)
{
	TArray<int> Result;
	Result.Init(-1, Pattern.Len());
	for (int r = 1, l = -1; r < Pattern.Len(); r++)
	{
		while (l != -1 && Pattern[l + 1] != Pattern[r])
			l = Result[l];
		if (Pattern[l + 1] == Pattern[r])
			Result[r] = ++l;
	}
	return Result;
}

TOptional<FString> FDictionary::FindNextWord(TSearchTask& Task, const FThreadSafeCounter& RequestCounter)
{
	static const int IterationBeforeCheck = 100; // Parameter
	int IterationCounter = 0;
	for (int i = Task.NextIndexToCheck; i < DictionaryStringArray.Num(); ++i)
	{
		++IterationCounter;
		if (IterationCounter == IterationBeforeCheck)
		{
			if (RequestCounter.GetValue() != Task.TaskId)
			{
				return TOptional<FString>();
			}
			IterationCounter = 0;
		}
		if (IsSatisfiesRequest(DictionaryStringArray[i], Task.Request, Task.PArray))
		{
			Task.NextIndexToCheck = i + 1;
			return DictionaryStringArray[i];
		}
	}
	Task.NextIndexToCheck = DictionaryStringArray.Num();
	Task.bIsCompleteSearching = true;
	return TOptional<FString>();
}

bool FDictionary::IsSatisfiesRequest(const FString& StringInWhichWeFindPattern, const FString& Pattern,
                                     const TArray<int>& PArray)
{
	int tail = -1;

	for (int i = 0; i < StringInWhichWeFindPattern.Len(); i++)
	{
		while (tail != -1 && StringInWhichWeFindPattern[i] != Pattern[tail + 1])
			tail = PArray[tail];
		if (StringInWhichWeFindPattern[i] == Pattern[tail + 1])
			tail++;
		if (tail == Pattern.Len() - 1)
		{
			return true;
		}
	}
	return false;
}
#endif