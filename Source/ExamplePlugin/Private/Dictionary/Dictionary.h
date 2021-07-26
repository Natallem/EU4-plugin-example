#pragma once
// #include "Multithreading/Searcher.h"

class FDictionary
{
public:
	FDictionary();
	void GetTxtData(FString path);
	TOptional<FString> FindNextWord(class FTask& Task, const FThreadSafeCounter& RequestCounter);

	static TArray<int> CreatePArray(const FString& pattern);
	static bool IsSatisfiesRequest(const FString& StringInWhichWeFindPattern, const FString& Pattern, const TArray<int> &PArray);
private:
	const FString Path = TEXT(R"(C:\Projects\UnrealEngineProjects\ExampleProject\ExampleProject\Plugins\ExamplePlugin\Resources\words.txt)");
	TArray<FString> DictionaryStringArray;
};
	
