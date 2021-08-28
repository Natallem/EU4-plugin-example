#pragma once

#include "CoreMinimal.h"
#include "Multithreading//SearchableItem.h"
#include "Templates/SharedPointer.h"
#include "PropertyEditor/Private/SDetailsViewBase.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSearchSettingDetail, Log, All);

class SWidget;
class SDetailsView;

/** Class is used common for all settings details of different types (category, section, inner category and properties).
 * Contains common logic for all types.
 * All inherited Details are ordered and contains SharedPtr to previous Detail type
 * (Category->Section->Inner category->Property), thus can use data from higher detail type. 
 */
class FAbstractSettingDetail : public ISearchableItem
{
public:
	/* Used for opening settings tab in proper category and section. FCategoryDetail and FSectionDetail should give their name (not the same, as their display name) */


	/** Uses for opening settings tab with proper category and section. FCategoryDetail and FSectionDetail should give their name (not the same, as their display name)
	 * @return inner name, that is searchable in property editor sections and categories map 
	 */
	virtual FName GetName() const;


	/** Used to be shown in plugin window
	 * @return displayed path of this setting detail
	 */
	virtual FString GetPath() const = 0;

	/** ISearchableItem interface */
	virtual TSharedRef<SWidget> GetRowWidget(FText Request) const override;

protected:
	/** SDetailsView is private class, that is responsible to store all nodes of properties (as they have hierarchical order) and do stuff with them. 
	 *@return  private field SSettingsEditor::SettingsView of SSettingsEditor object that is the content of currently opened UE Preference Settings Window (i.e. Edit|Editor Preferences).
	 */
	TSharedPtr<SDetailsView> GetSDetailsView() const;

	/** @return private field SDetailsView::DetailTree that is in SDetailsViewBase. SDetailTree is representation of currently shown properties in SDetailsView */
	TSharedPtr<SDetailTree> GetDetailTree(TSharedPtr<SDetailsView>) const;

	/** Sets the Text into Search box in Settings window */
	void SetTextInSearchBox(TSharedPtr<SDetailsView> DetailsViewPtr, const FText& newText);

	/** Delimiter is separates items in displayed settings path */
	FString Delimiter = "|";
};
