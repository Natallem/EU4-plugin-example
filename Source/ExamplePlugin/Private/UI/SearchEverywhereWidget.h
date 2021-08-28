#pragma once

#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "SettingsData/PropertyHolder.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "SettingsData/SettingItemTypes.h"

class SEditableText;
class SSearchEverywhereWindow;
class FSearcher;
class STextBlock;
class ITableRow;
class STableViewBase;
struct FItemFoundMsg;

template <typename T>
class SListView;

class SSearchEverywhereWidget final : public SCompoundWidget
{
private:
	typedef TSharedPtr<TOptional<TSharedRef<ISearchableItem>>> FListItemPtr;
	typedef SListView<FListItemPtr> SListViewWidget;
public:
	SLATE_BEGIN_ARGS(SSearchEverywhereWidget)
		{
		}

		SLATE_ARGUMENT(FText, PreviousSearchRequest)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<SSearchEverywhereWindow> InParentWindow);

	virtual bool SupportsKeyboardFocus() const override;
	FText GetCurrentSearchRequest() const;

	/** Depending on the type of InMessage change appearance of ItemsListView (i.e. add or hide "More" button, add new found elements) */
	void ProcessMessage(const FItemFoundMsg& InMessage);

private:
	FSlateColor GetButtonColor(ESettingType ButtonType) const;

	EActiveTimerReturnType SetKeyboardFocus(double InCurrentTime, float InDeltaTime) const;

	TSharedRef<ITableRow> OnGenerateTabSwitchListItemWidget(FListItemPtr InItem,
	                                                        const TSharedRef<STableViewBase>& OwnerTable) const;

	void OnListSelectionChanged(FListItemPtr InItem, ESelectInfo::Type SelectInfo);

	/** Changes selected item and set default select item the first one
	 * @param bIsMoving if user pressed up or down key and we need to move selection up or down
	 * @param bIsDownMoving direction of movement
	 */
	void CycleSelection(bool bIsMoving = false, bool bIsDownMoving = false);

	void OnTextChanged(const FText& Filter);

	void OnTextCommit(const FText& CommittedText, ETextCommit::Type CommitType) const;

	FReply OnSearchTextKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent);

	void OnSelectItem(FListItemPtr InItem) const;

	FReply ChangeSettingSearchType(ESettingType);

	FPropertyHolder& PropertyHolder = FPropertyHolder::Get();
	TArray<FListItemPtr> ItemsSource;
	TSharedPtr<FSearcher> Searcher;
	TSharedPtr<SEditableText> SearchEditableText;
	TSharedPtr<SListViewWidget> ItemsListView;
	TSharedPtr<STextBlock> ShowMoreResultsItem;
	TSharedPtr<SSearchEverywhereWindow> ParentWindow;
	bool bShouldCleanList = false;
	int64 SelectedListViewItemIndex = 0;
	bool bIsFirstFocusActivate = true;
	ESettingType SearchSettingType = All;
};
