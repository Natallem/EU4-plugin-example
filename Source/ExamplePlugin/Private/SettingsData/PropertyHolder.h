#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "Multithreading/Configuration.h"

class SWidget;
class ISettingsModule;
class FAbstractSettingDetail;
template <typename Type>
class TSearchTask;

class FPropertyHolder
{
public:
	using FSearchTask = TSearchTask<RequiredType>;

	static FPropertyHolder& Get();
	
	static TArray<int> CreatePArray(const FString& InPattern);
	TOptional<RequiredType> FindNextWord(FSearchTask& OutTask, const FThreadSafeCounter& InRequestCounter);
	TSharedRef<const FAbstractSettingDetail> GetSettingDetail(uint64 Index) const;

	void LogAllProperties() const;

private:
	FPropertyHolder();
	void ForceUpdateSettings() const;
	void LoadProperties();
	template <typename T>
	T AddToPropertyHolder(const T& Item);
	
	static void WriteLog(const FString& Text, bool bIsAppend = true);
	static bool IsSatisfiedRequest(const FString& InStringToFindPattern, const FString& InPattern,
	                               const TArray<int>& PArray);
	
	TArray<FString> SettingDetailsNames;
	TArray<TSharedRef<FAbstractSettingDetail>> SettingDetails;
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
};
