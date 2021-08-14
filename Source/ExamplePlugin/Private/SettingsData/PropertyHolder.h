#pragma once

#include "Multithreading/Configuration.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class SWidget;
class ISettingsModule;

template <typename Type>
class TSearchTask;
class FAbstractSettingDetail;

class FPropertyHolder
{
public:
	using FSearchTask = TSearchTask<RequiredType>;

	static TArray<int> CreatePArray(const FString& Pattern);
	TOptional<RequiredType> FindNextWord(FSearchTask& Task, const FThreadSafeCounter& RequestCounter);
	TSharedRef<const FAbstractSettingDetail> GetSettingDetail(uint64 Index) const;

	static void LogAllProperties();
	static void WriteLog(const FString& Text, bool IsAppend = true);
	static FPropertyHolder& Get();
private:
	void ForceUpdateSettings() const;
	FPropertyHolder();
	template <typename T>
	T AddToPropertyHolder(const T& Item);

	void LoadProperties();

	static bool IsSatisfiesRequest(const FString& StringInWhichWeFindPattern, const FString& Pattern,
	                               const TArray<int>& PArray);
	TArray<FString> SettingDetailsNames;
	TArray<TSharedRef<FAbstractSettingDetail>> SettingDetails;
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
};
