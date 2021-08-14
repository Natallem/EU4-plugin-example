#include "SearchEverywhereWidget.h"

#include "EditorStyleSet.h"
#include "IDetailTreeNode.h"
#include "IPropertyRowGenerator.h"
#include "ISettingsCategory.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "PropertyPath.h"
#include "STextPropertyEditableTextBox.h"
#include "Misc/FileHelper.h"
#include "Multithreading/Searcher.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/Layout/SScrollBox.h"
// #include "SourceCodeAccess/Private/SourceCodeAccessSettings.h"
#include "Widgets/SWindow.h"
#include "Classes/EditorStyleSettings.h"
#include "Framework/Application/SlateApplication.h"
#include "SettingsData/PropertyHolder.h"
#include "PropertyEditor/Private/DetailItemNode.h"
#include "PropertyEditor/Private/SDetailsView.h"
#include "SettingsData/AbstractSettingDetail.h"
#include "Widgets/Input/SEditableText.h"
#include "Templates/SharedPointer.h"

#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void SSearchEverywhereWidget::Construct(const FArguments& InArgs, TSharedRef<SWindow> InParentWindow,
                                        TSharedRef<FSearcher> SearcherArgument)
{
	Searcher = SearcherArgument;
	ParentWindow = InParentWindow;
	ListView = SNew(SListViewWidget)
		.IsFocusable(true)
		.ItemHeight(64)
		.SelectionMode(ESelectionMode::None)
		.ListItemsSource(&StringItems)
		.OnGenerateRow(this, &SSearchEverywhereWidget::OnGenerateTabSwitchListItemWidget)
		.OnMouseButtonClick_Lambda([this](FListItemPtr InItem)
	                                {
		                                PropertyHolder.GetSettingDetail(InItem->GetValue())->DoAction();
		                                UE_LOG(LogTemp, Log, TEXT("EP : OnMouseButtonClick_Lambda Close"));
ParentWindow->RequestDestroyWindow();
		                                // FSlateApplication::Get().RequestDestroyWindow(ParentWindow.ToSharedRef());
		                                // StaticCastSharedPtr<SWindow>(GetParentWidget())->RequestDestroyWindow();
	                                })
	.SelectionMode(ESelectionMode::Single);


	ListTableWidget = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SScrollBorder, ListView.ToSharedRef())
			[
				ListView.ToSharedRef()
			]
		];

	SAssignNew(ShowMoreResultsButton, SButton)
			.Text(LOCTEXT("ShowMoreResultsButtonText", "More ..."))
			.Visibility(EVisibility::Visible)
			.OnClicked(this, &SSearchEverywhereWidget::OnButtonShowMoreResultsClicked);

	const TSharedRef<SWidget> TabsWidget = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("Button1Text", "Button1"))
			// .OnClicked(FOnClicked::CreateStatic(&SSearchEverywhereWidget::OpenSettings, FName("Editor"),
			// FName("General"), FName("Appearance")))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("Button2Text", "Button2"))
			.OnClicked(this, &SSearchEverywhereWidget::GetAllProperties)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("Button3Text", "Button3"))
			.OnClicked(this, &SSearchEverywhereWidget::OpenSettings, FName("Editor"), FName("General"),
			           FName("Appearance"))
		]


		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ContentPadding(0.4)
			.ClickMethod(EButtonClickMethod::MouseDown)
			.Text(LOCTEXT("Button4Text", "Button4"))
		];

	const TSharedRef<SWidget> SearchTableWidget =
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .Padding(10.0f)
		  .FillWidth(1.0f)
		[
			SAssignNew(EditableTextBox, SEditableText)
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
					ListTableWidget.ToSharedRef()
				]
			]
		]
	];
}

void SSearchEverywhereWidget::UpdateShownResults()
{
	TPair<bool, TArray<RequiredType>> Result = Searcher->GetRequestData();
	if (ShouldCleanList)
	{
		ShouldCleanList = false;
		StringItems.Reset();
	}

	if (StringItems.Num() > 0 && !StringItems.Last()->IsSet())
	{
		StringItems.Pop(false);
	}
	for (RequiredType& Str : Result.Value)
	{
		StringItems.Add(MakeShared<TOptional<RequiredType>>(Str));
	}
	if (!Result.Key)
	{
		StringItems.Add(MakeShared<TOptional<RequiredType>>());
	}
	ListView->RebuildList();
}

FReply SSearchEverywhereWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

void SSearchEverywhereWidget::OnTextChanged(const FText& Filter)
{
	ShouldCleanList = true; //todo maybe just clean?
	Searcher->SetInput(Filter.ToString());
}

FReply SSearchEverywhereWidget::OnButtonShowMoreResultsClicked() const
{
	if (ShowMoreResultsButton->GetVisibility() == EVisibility::Visible)
	{
		Searcher->FindMoreDataResult();
	}
	return FReply::Handled(); // todo or unhandled?
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
	TSharedPtr<SWidget> InnerWidget = ShowMoreResultsButton;
	if (InItem->IsSet())
	{
		InnerWidget = PropertyHolder.GetSettingDetail(InItem->GetValue())->GetRowWidget();
		// InnerWidget = SNew(STextBlock)
		// .Text(FText::FromString(*InItem->GetValue()));
	}
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			InnerWidget.ToSharedRef()
		]
	];
}

FReply SSearchEverywhereWidget::OpenSettings(FName InContainerName, FName InCategoryName, FName InSectionName)
{
	SettingsModule.ShowViewer(InContainerName, InCategoryName, InSectionName);
	return FReply::Handled();
}

FReply SSearchEverywhereWidget::GetAllProperties()
{
	FPropertyHolder::LogAllProperties();
	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE
