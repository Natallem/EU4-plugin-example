#pragma once

class FPropertyHolder
{
public:
	void LoadProperties();
	FPropertyHolder();
	static TArray<int> CreatePArray(const FString& Pattern);
	TOptional<FString> FindNextWord(class FSearchTask& Task, const FThreadSafeCounter& RequestCounter);

	static void LogAllProperties();
	static void WriteLog(const FString& Text, int LogNumber, bool IsAppend = true);

private:
	static bool IsSatisfiesRequest(const FString& StringInWhichWeFindPattern, const FString& Pattern,
	                               const TArray<int>& PArray);
	TArray<FString> PropertyNameArray;
	TArray<ISettingDetail> PropertyMap;
	class ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");

};
