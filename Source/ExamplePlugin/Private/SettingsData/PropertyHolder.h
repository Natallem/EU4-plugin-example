#pragma once

#include "CoreMinimal.h"
#include "ISettingsEditorModel.h"
#include "Modules/ModuleManager.h"
class FInputHandler;
class ISearchableItem;
// #include "Multithreading/InputHandler.h"
class ISettingsCategory;
class ISettingsModule;
class FAbstractSettingDetail;
class FCategoryDetail;
class FDetailCategoryImpl;
class FDetailMultiTopLevelObjectRootNode;
class FDetailTreeNode;
class FInnerCategoryDetail;
class FPropertyDetail;
class FSectionDetail;
class SWidget;
class SDockTab;

DECLARE_LOG_CATEGORY_EXTERN(LogPropertyHolder, Log, All)

struct FSettingsDataCollection
{
	FSettingsDataCollection() = default;

	FSettingsDataCollection(const TArray<TSharedRef<FCategoryDetail>>& CategoryDetails,
	                        const TArray<TSharedRef<FSectionDetail>>& SectionDetails,
	                        const TArray<TSharedRef<FInnerCategoryDetail>>& InnerCategoryDetails,
	                        const TArray<TSharedRef<FPropertyDetail>>& PropertyDetails);

	TArray<TSharedRef<FCategoryDetail>> CategoryDetails;
	TArray<TSharedRef<FSectionDetail>> SectionDetails;
	TArray<TSharedRef<FInnerCategoryDetail>> InnerCategoryDetails;
	TArray<TSharedRef<FPropertyDetail>> PropertyDetails;

	int Num() const;;

	TSharedRef<FAbstractSettingDetail> operator[](int Index) const;
};

class FPropertyHolder
{
public:

	static FPropertyHolder& Get();

	TOptional<TSharedRef<ISearchableItem>> FindNextWord(const TSharedPtr<FInputHandler, ESPMode::ThreadSafe>& InputTask) const;

	TSharedRef<FAbstractSettingDetail> GetSettingDetail(uint64 Index) const;
	template <bool bShouldLog>
	static FSettingsDataCollection GetSettingsData(ISettingsModule& SettingModule = GetSettingModule());

	static ISettingsModule& GetSettingModule();

private:
	FPropertyHolder();

	/** Opens Setting Editor Tab to extract all settings properties from it. Need to be closed
	 * @brief 
	 * @param EditorTab that will be opened by Editor
	 * @param SettingsModule used to open tab 
	 * @return whether successfully opened tab
	 */
	static bool OpenEditorTab(TSharedPtr<SDockTab>& EditorTab, ISettingsModule& SettingsModule);

	template <bool bShouldLog>
	static TSharedPtr<FSectionDetail> FindSectionByObjectAndCheck(
		TSharedRef<FDetailMultiTopLevelObjectRootNode> SectionNode,
		TMap<TSharedPtr<ISettingsCategory>, TSharedPtr<FCategoryDetail>>& FoundCategories,
		FSettingsDataCollection& OutResult, ISettingsModule& SettingModule, const ISettingsEditorModelPtr& Model,
		int DetailLayoutIndex);

	template <bool bShouldLog>
	static void FindAllInnerCategoryDetails(const TSharedRef<FDetailMultiTopLevelObjectRootNode>& SectionNode,
	                                        const TSharedRef<FSectionDetail>& SectionDetail,
	                                        FSettingsDataCollection& OutResult);

	template <bool bShouldLog>
	static void FindAllPropertyDetails(TSharedRef<FDetailCategoryImpl>& CategoryNode,
	                                   TSharedRef<FInnerCategoryDetail>& InnerCategoryDetail,
	                                   FSettingsDataCollection& OutResult);

	static FString GetPropertyName(TSharedRef<FDetailTreeNode> DetailNode);

	static void WriteLog(const FString& Text, bool bIsAppend = true);

	FSettingsDataCollection Data;
	static const FName EditorSettingsName;
};
