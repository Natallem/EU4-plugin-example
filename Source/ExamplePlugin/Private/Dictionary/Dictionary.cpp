#include "Dictionary.h"

#include "Multithreading/SearchTask.h"
#include "Misc/FileHelper.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"

FDictionary::FDictionary()
{
	GetTxtData(Path);
}

void FDictionary::GetTxtData(FString path)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*path))
	{
		FString str = path + "File does not exist!";
		UE_LOG(LogTemp, Warning, TEXT("%s, file does not exist!"), *str);
		return;
	}
	FFileHelper::LoadANSITextFileToStrings(*path, NULL, DictionaryStringArray);
}

TOptional<FString> FDictionary::FindNextWord(FSearchTask& Task,  const FThreadSafeCounter& RequestCounter)
{
	static const int IterationBeforeCheck = 100; // Parameter
	int IterationCounter = 0;
	UE_LOG(LogTemp, Log, TEXT("EP : FDictionary index=%d input=%s"), Task.NextIndexToCheck, &Task.Request);
	for (int i = Task.NextIndexToCheck; i < DictionaryStringArray.Num(); ++i)
	{
		++IterationCounter;
		if (IterationCounter == IterationBeforeCheck)
		{
			if (RequestCounter.GetValue() != Task.TaskId)
			{
				UE_LOG(LogTemp, Log, TEXT("EP : FDictionary id changed"));

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

TArray<int> FDictionary::CreatePArray(const FString& pattern)
{
	TArray<int> Result;
	Result.Init(-1, pattern.Len());
	for (int r = 1, l = -1; r < pattern.Len(); r++)
	{
		while (l != -1 && pattern[l + 1] != pattern[r])
			l = Result[l];
		if (pattern[l + 1] == pattern[r])
			Result[r] = ++l;
	}
	return Result;
}
