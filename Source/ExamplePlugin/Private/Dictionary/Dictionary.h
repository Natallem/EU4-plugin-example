#pragma once

class FDictionary
{
public:
	FDictionary();
	void GetTxtData(const FString& Path);
	static TArray<int> CreatePArray(const FString& Pattern);
	TOptional<FString> FindNextWord(class FSearchTask& Task, const FThreadSafeCounter& RequestCounter);
private:
	static bool IsSatisfiesRequest(const FString& StringInWhichWeFindPattern, const FString& Pattern,
	                               const TArray<int>& PArray);
	const FString DictionaryPath = TEXT(
		R"(C:\Projects\UnrealEngineProjects\ExampleProject\ExampleProject\Plugins\ExamplePlugin\Resources\words.txt)");
	TArray<FString> DictionaryStringArray;
};
