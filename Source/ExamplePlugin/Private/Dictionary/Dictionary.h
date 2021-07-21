#pragma once
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"

class FDictionary
{
public:
	FDictionary();
	void GetTxtData(FString path);

private:
	bool IsSatisfiesRequest(const FString& Word, const FString& String, const TArray<int> &);
	static TArray<int> CreatePArray(FString& pattern);

	TArray<FString> FindResultForRequest(FString && Request);
	const FString Path = TEXT(R"(C:\Projects\UnrealEngineProjects\ExampleProject\ExampleProject\Plugins\ExamplePlugin\Resources\words.txt)");
	TArray<FString> DictionaryStringArray;
};
	
