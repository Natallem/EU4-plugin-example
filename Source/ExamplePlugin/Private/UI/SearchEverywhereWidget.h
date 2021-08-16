#pragma once

#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "Multithreading/Configuration.h"
#include "SettingsData/PropertyHolder.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class SEditableText;
class SSearchEverywhereWindow;
class FSearcher;
class STextBlock;
class ITableRow;
class STableViewBase;

template <typename T>
class SListView;

class SSearchEverywhereWidget final : public SCompoundWidget
{
private:
	typedef TSharedPtr<TOptional<RequiredType>> FListItemPtr;
	typedef SListView<FListItemPtr> SListViewWidget;
public:
	SLATE_BEGIN_ARGS(SSearchEverywhereWidget)
		{
		}

		SLATE_ARGUMENT(FText, PreviousSearchRequest)
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs, TSharedRef<SSearchEverywhereWindow> InParentWindow);

	void UpdateShownResults();
	virtual bool SupportsKeyboardFocus() const override;
	FText GetCurrentSearchRequest() const;
protected:
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
private:
	EActiveTimerReturnType SetFocusPostConstruct(double InCurrentTime, float InDeltaTime) const;
	TSharedRef<ITableRow> OnGenerateTabSwitchListItemWidget(FListItemPtr InItem,
	                                                        const TSharedRef<STableViewBase>& OwnerTable) const;
	void OnListSelectionChanged(FListItemPtr InItem, ESelectInfo::Type SelectInfo);
	void CycleSelection(bool bIsMoving = false, bool bIsDownMoving = false);

	void OnTextChanged(const FText& Filter);
	void OnTextCommit(const FText& CommittedText, ETextCommit::Type CommitType) const;
	FReply OnSearchTextKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent);
	void OnSelectItem(FListItemPtr InItem) const;
	FReply OpenSettings(FName ContainerName, FName CategoryName, FName SectionName) const;
	static FReply LogAllProperties();

	FPropertyHolder& PropertyHolder = FPropertyHolder::Get();
	TArray<FListItemPtr> ItemsSource;
	TSharedPtr<FSearcher> Searcher;
	TSharedPtr<SEditableText> SearchEditableText;
	TSharedPtr<SListViewWidget> ItemsListView;
	TSharedPtr<STextBlock> ShowMoreResultsItem;
	TSharedPtr<SWidget> ListTableWidget;
	TSharedPtr<SSearchEverywhereWindow> ParentWindow;
	bool ShouldCleanList = false;
	int64 SelectedListViewItemIndex = 0;
	bool bIsFirstFocusActivate = true;
};
