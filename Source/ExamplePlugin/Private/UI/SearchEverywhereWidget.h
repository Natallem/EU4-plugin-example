#pragma once
#include "Widgets/Input/SButton.h"


class SSearchEverywhereWidget final : public SCompoundWidget
{
private:
	typedef TSharedPtr<TOptional<FString>> FListItemPtr;
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
	FReply FirstAttemptToGetAllProperties();
	FReply SecondAttemptToGetAllProperties();
	FReply ThirdAttemptToGetAllProperties();

	void HandlePropertiesChanged();
	void WriteLog(const FString& Text, int LogNumber, bool IsAppend = true);
	void ProcessIDetailTreeNode(TSharedRef<IDetailTreeNode>& Node, const FString& Tabs = "");

	TSharedRef<ITableRow> OnGenerateTabSwitchListItemWidget(FListItemPtr InItem,
	                                                        const TSharedRef<STableViewBase>& OwnerTable);
	void OnTextChanged(const FText& Filter);
	FReply OnButtonShowMoreResultsClicked() const;

	class ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
	TSharedPtr<SEditableText> EditableText;
	TSharedPtr<SListViewWidget> ListView;
	TArray<FListItemPtr> StringItems;
	TSharedPtr<SButton> ShowMoreResultsButton;
	TSharedPtr<class SEditableText> EditableTextBox;
	TSharedPtr<FSearcher> Searcher;
	TSharedPtr<SWidget> ListTableWidget;
	bool ShouldCleanList = false;
};
