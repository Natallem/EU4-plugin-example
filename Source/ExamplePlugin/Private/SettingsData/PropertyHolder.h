#pragma once

#include "ISettingsCategory.h"
#include "ISettingsSection.h"

class ISettingDetail;
class ISettingsModule;

class FPropertyHolder
{
public:
	using ISettingDetailRef = TSharedRef<ISettingDetail>;
	
	FPropertyHolder();
	static TArray<int> CreatePArray(const FString& Pattern);
	TOptional<uint64> FindNextWord(class FSearchTask& Task, const FThreadSafeCounter& RequestCounter);

	static void LogAllProperties();
	static void WriteLog(const FString& Text, int LogNumber, bool IsAppend = true);

private:
	template <typename T>
	T& AddToPropertyHolder(const T& Item);
	void LoadProperties();

	static bool IsSatisfiesRequest(const FString& StringInWhichWeFindPattern, const FString& Pattern,
	                               const TArray<int>& PArray);
	TArray<FString> SettingDetailsNames;
	TArray<ISettingDetailRef> SettingDetails;
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
};
