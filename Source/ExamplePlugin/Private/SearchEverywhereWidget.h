#pragma once


class SSearchEverywhereWidget final : public SCompoundWidget
{	
private:
	typedef TSharedPtr<FString> FListItemPtr;
	typedef SListView<FListItemPtr> SListViewWidget;

public:
	SLATE_BEGIN_ARGS(SSearchEverywhereWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	TSharedRef<ITableRow> OnGenerateTabSwitchListItemWidget(FListItemPtr InItem, const TSharedRef<STableViewBase>& OwnerTable);

private:
	FReply OnButtonClicked();
	void OnTextChanged(const FText& Filter);
	TSharedRef<ITableRow> OnGenerateWidgetForList(FListItemPtr InItem, const TSharedRef<STableViewBase>& OwnerTable );

	TSharedPtr< SEditableText > EditableText;
	TSharedPtr<SListViewWidget> ListView;
	TArray<FListItemPtr> StringItems;
	TSharedPtr<class SEditableText> EditableTextBox;
};

