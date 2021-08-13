#pragma once

#include "Multithreading/Configuration.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class SWidget;
class ISettingDetail;
class ISettingsModule;

template <typename Type>
class TSearchTask;

class FPropertyHolder
{
public:
	using FSearchTask = TSearchTask<RequiredType>;
	
	static TArray<int> CreatePArray(const FString& Pattern);
	TOptional<RequiredType> FindNextWord(FSearchTask& Task, const FThreadSafeCounter& RequestCounter);
	TSharedPtr<SWidget> GetPropertyWidgetForIndex(uint64 Index);

	static void LogAllProperties();
	static void WriteLog(const FString& Text, int LogNumber, bool IsAppend = true);
	static FPropertyHolder& Get();
private:
	FPropertyHolder();
	template <typename T>
	T AddToPropertyHolder(const T& Item);
	
	void LoadProperties();

	static bool IsSatisfiesRequest(const FString& StringInWhichWeFindPattern, const FString& Pattern,
	                               const TArray<int>& PArray);
	TArray<FString> SettingDetailsNames;
	TArray<TSharedRef<ISettingDetail>> SettingDetails;
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
};
