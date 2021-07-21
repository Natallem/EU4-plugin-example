#include "SearchEverywhereWidget.h"

#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/STextEntryPopup.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "STextPropertyEditableTextBox.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void SSearchEverywhereWidget::Construct(const FArguments& InArgs)
{
	int ElementNumber = 30;
	StringItems.Reserve(ElementNumber);
	for (int i = 0; i < ElementNumber; i++)
	{
		FString str("String number ");
		str.AppendInt(i);
		str.AppendChar('\n');
		StringItems.Add(MakeShared<FString>(str));
	}
	ListView = SNew(SListViewWidget)
		.IsFocusable(true)
		.ItemHeight(64)
		.SelectionMode(ESelectionMode::None)
		.ListItemsSource(&StringItems)
		.OnGenerateRow(this, &SSearchEverywhereWidget::OnGenerateTabSwitchListItemWidget);

	const TSharedRef<SWidget> ListTableWidget = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SScrollBorder, ListView.ToSharedRef())
			[
				ListView.ToSharedRef()
			]
		];

	const TSharedRef<SWidget> TabsWidget = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("Button1Text", "Button1"))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.OnClicked(this, &SSearchEverywhereWidget::OnButtonClicked)
			.Text(LOCTEXT("Button2Text", "Button2"))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("Button3Text", "Button3"))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("Button4Text", "Button4"))
		];

	const TSharedRef<SWidget> SearchTableWidget =
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(10.0f)
		.FillWidth(1.0f)
		[
			SAssignNew(EditableTextBox, SEditableText)
			.Text(LOCTEXT("DisabledContextMenuInput", "Type here to search ... "))
			.RevertTextOnEscape(true)
			.OnTextChanged(this, &SSearchEverywhereWidget::OnTextChanged)
		];

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::Get().GetBrush("ControlTabMenu.Background"))
		.ForegroundColor(FCoreStyle::Get().GetSlateColor("DefaultForeground"))
		[
			SNew(SBox)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Left)
				.AutoHeight()
				[
					TabsWidget
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SearchTableWidget
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.Padding(0)
				[
					ListTableWidget
				]
			]
		]
	];
}

FReply SSearchEverywhereWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

void SSearchEverywhereWidget::OnTextChanged(const FText& Filter)
{
	int x = 10;
	EditableTextBox->SetText(Filter);
}


void SSearchEverywhereWidget::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWidget OnFocusLost"));

	SCompoundWidget::OnFocusLost(InFocusEvent);
}

FReply SSearchEverywhereWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWidget OnFocusReceived"));

	return SCompoundWidget::OnFocusReceived(MyGeometry, InFocusEvent);
}


void SSearchEverywhereWidget::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath,
                                              const FWidgetPath& NewWidgetPath)
{
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWidget OnFocusChanging"));

	SCompoundWidget::OnFocusChanging(PreviousFocusPath, NewWidgetPath);
}


TSharedRef<ITableRow> SSearchEverywhereWidget::OnGenerateTabSwitchListItemWidget(FListItemPtr InItem,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(FText::FromString(*InItem))
		]
	];
}

FReply SSearchEverywhereWidget::OnButtonClicked()
{
	static int i = 100;
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWidget Button Clicked"));
	FString str("String number ");
	str.AppendInt(i++);
	str.AppendChar('\n');
	StringItems.Add(MakeShared<FString>(str));
	ListView->RebuildList();
	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE
