#pragma once
#include "Modules/ModuleManager.h"
#include "Widgets/Input/SButton.h"
#include "Multithreading/Configuration.h"
#include "SettingsData/PropertyHolder.h"
#include "ISettingsModule.h"

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

	void Construct(const FArguments& InArgs, TSharedRef<class FSearcher> Searcher);

	void UpdateShownResults();
protected:
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

	FReply OpenSettings(FName ContainerName, FName CategoryName, FName SectionName);

private:
	// USourceCodeAccessSettings * USourceCodeAccessSettingsPtr;
	FReply GetAllProperties();

	TSharedRef<ITableRow> OnGenerateTabSwitchListItemWidget(FListItemPtr InItem,
	                                                        const TSharedRef<STableViewBase>& OwnerTable);
	void OnTextChanged(const FText& Filter);
	FReply OnButtonShowMoreResultsClicked() const;

	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
	TSharedPtr<SEditableText> EditableText;
	TSharedPtr<SListViewWidget> ListView;
	TArray<FListItemPtr> StringItems;
	TSharedPtr<SButton> ShowMoreResultsButton;
	TSharedPtr<SEditableText> EditableTextBox;
	TSharedPtr<FSearcher> Searcher;
	TSharedPtr<SWidget> ListTableWidget;
	bool ShouldCleanList = false;
	FPropertyHolder& PropertyHolder = FPropertyHolder::Get();
};
