#pragma once

#include "CoreMinimal.h"
#include "ISettingsEditorModel.h"
#include "SettingItemTypes.h"
#include "Engine/EngineTypes.h"
#include "Modules/ModuleManager.h"
class SDetailsView;
class FInputData;
class ISearchableItem;
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

	int Num() const;

	TSharedRef<FAbstractSettingDetail> operator[](int Index) const;

	/** Type of current Input holder. Switch types of Details to Iterate throw */
	ESettingType SettingType = All;
};

class FPropertyHolder
{
public:
	/** Uses FPropertyHolder as standalone */
	static FPropertyHolder& Get();

	/** Try to find next item, corresponding to request in InputTask. Uses FInputData::NextIndexToCheck as start
	 * position for searching and increments it in case item was found or no more items can be found ever. Also checks
	 * while searching that Input was not changed and InputData was not cancelled. 
	 * @return Next satisfying item or Nothing if there is no more unchecked items in Holder or Input is cancelled
	 */
	TOptional<TSharedRef<ISearchableItem>> FindNextItem(
		const TSharedPtr<FInputData, ESPMode::ThreadSafe>& InputData);

	/** Finds new valid TreeNode in Settings Window for FPropertyDetail::PropertyDetailTreeNode and
	 *FInnerCategoryDetail::CategoryTreeNode. Used in cases then Settings window tab changed and previous TreeNodes are invalid.
	 * @return wether Settings window is accessible and data from it collected successfully
	 */
	bool UpdateTreeNodes(
		const TSharedRef<FPropertyDetail> PropertyDetail, TSharedRef<FInnerCategoryDetail> InnerCategoryDetail);

	/** Get all types of settings data from Settings Editor Window. Uses private field to access property information.
	 *This function is template in case of Log found properties (use FPropertyHolder::Get().GetSettingsData<true>())*/
	template <bool bShouldLog>
	static FSettingsDataCollection GetSettingsData(ISettingsModule& SettingModule = GetSettingModule());

	static ISettingsModule& GetSettingModule();

private:
	FPropertyHolder();

	static TSharedPtr<SDetailsView> GetDetailsView(TSharedPtr<SDockTab>& EditorSettingsTab, bool& bShouldCloseSettings,
	                                               ISettingsModule& SettingModule);

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
