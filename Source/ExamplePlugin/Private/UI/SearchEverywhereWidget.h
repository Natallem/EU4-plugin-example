#pragma once

#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "Multithreading/Configuration.h"
#include "SettingsData/PropertyHolder.h"
#include "Widgets/Input/SButton.h"

class SEditableText;
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
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<SWindow> InParentWindow, TSharedRef<class FSearcher> Searcher);

	void UpdateShownResults();
	virtual bool SupportsKeyboardFocus() const override;
protected:
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

	FReply OpenSettings(FName ContainerName, FName CategoryName, FName SectionName);

private:
	EActiveTimerReturnType SetFocusPostConstruct(double InCurrentTime, float InDeltaTime) const;
	FReply GetAllProperties();
	TSharedRef<ITableRow> OnGenerateTabSwitchListItemWidget(FListItemPtr InItem,
	                                                        const TSharedRef<STableViewBase>& OwnerTable);
	void OnTextChanged(const FText& Filter);
	FReply OnButtonShowMoreResultsClicked() const;

	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
	TSharedPtr<SListViewWidget> ListView;
	TArray<FListItemPtr> ItemsSource;
	TSharedPtr<SButton> ShowMoreResultsButton;
	TSharedPtr<SEditableText> SearchEditableText;
	TSharedPtr<FSearcher> Searcher;
	TSharedPtr<SWidget> ListTableWidget;
	bool ShouldCleanList = false;
	FPropertyHolder& PropertyHolder = FPropertyHolder::Get();
	TSharedPtr<SWindow> ParentWindow;
};
