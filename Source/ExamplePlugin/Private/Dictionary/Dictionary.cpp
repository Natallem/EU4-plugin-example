#include "Dictionary.h"

#include "Misc/FileHelper.h"

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

bool FDictionary::IsSatisfiesRequest(const FString& Word, const FString& Pattern, const TArray<int>& PArray)
{
	int tail = -1;
	
	for (int i = 0; i < Word.Len(); i++) {
		while (tail != -1 && Word[i] != Pattern[tail + 1])
			tail = PArray[tail];
		if (Word[i] == Pattern[tail + 1])
			tail++;
		if (tail == Pattern.Len() - 1) {
			return true;
		}
	}
	return false;
}

TArray<int> FDictionary::CreatePArray(FString& pattern)
{
	TArray<int> Result;
	Result.Reserve(pattern.Len());
	for (int i = 0; i < pattern.Len(); ++i)
	{
		Result[i] = -1;
	}
	for (int r = 1, l = -1; r < pattern.Len(); r++)
	{
		while (l != -1 && pattern[l + 1] != pattern[r])
			l = Result[l];
		if (pattern[l + 1] == pattern[r])
			Result[r] = ++l;
	}
	return Result;
}

TArray<FString> FDictionary::FindResultForRequest(FString&& Request)
{
	TArray<FString> Result;
	TArray<int> PArray = CreatePArray(Request);
	for (FString& Word : DictionaryStringArray)
	{
		if (IsSatisfiesRequest(Word, Request,))
		{
			Result.Push(Word);
		}
	}
	return Result;
}
