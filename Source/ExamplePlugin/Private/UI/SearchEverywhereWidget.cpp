#include "SearchEverywhereWidget.h"

#include "EditorStyleSet.h"
#include "Framework/Application/SlateApplication.h"
#include "Multithreading/Searcher.h"
// #include "SettingsData/PropertyHolder.h"
#include "SettingsData/AbstractSettingDetail.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/SWindow.h"

#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void SSearchEverywhereWidget::Construct(const FArguments& InArgs, TSharedRef<SWindow> InParentWindow,
                                        TSharedRef<FSearcher> SearcherArgument)
{
	Searcher = SearcherArgument;
	ParentWindow = InParentWindow;
	ListView = SNew(SListViewWidget)
		// .IsFocusable(true)
		.ItemHeight(64)
		.ListItemsSource(&ItemsSource)
		.SelectionMode(ESelectionMode::Single)
		.OnGenerateRow(this, &SSearchEverywhereWidget::OnGenerateTabSwitchListItemWidget)
		.OnMouseButtonClick_Lambda([this](FListItemPtr InItem)
	                                {
		                                if (InItem->IsSet())
		                                {
			                                PropertyHolder.GetSettingDetail(InItem->GetValue())->DoAction();
			                                ParentWindow->RequestDestroyWindow();
		                                }
	                                });


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
			.Text(LOCTEXT("OpenSettingsTestingTextButton", "Open Settings"))
			.OnClicked(this, &SSearchEverywhereWidget::OpenSettings, FName("Editor"),
			           FName("General"), FName("Appearance"))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("LogAllPropertiesButtonText", "Log all properties"))
			.OnClicked(this, &SSearchEverywhereWidget::GetAllProperties)
		];

	const TSharedRef<SWidget> SearchTableWidget =
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .Padding(10.0f)
		  .FillWidth(1.0f)
		[
			SAssignNew(SearchEditableText, SEditableText)
			.RevertTextOnEscape(true)
			.OnTextChanged(this, &SSearchEverywhereWidget::OnTextChanged)
			.SelectAllTextWhenFocused(true)
			.HintText(LOCTEXT("SearchEditableTextHint", "Type here ..."))
		];
	RegisterActiveTimer(
		0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SSearchEverywhereWidget::SetFocusPostConstruct));

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

EActiveTimerReturnType SSearchEverywhereWidget::SetFocusPostConstruct(double InCurrentTime, float InDeltaTime) const
{
	if (SearchEditableText.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(SearchEditableText.ToSharedRef(), EFocusCause::SetDirectly);
	}
	return EActiveTimerReturnType::Stop;
}

void SSearchEverywhereWidget::UpdateShownResults()
{
	TPair<bool, TArray<RequiredType>> Result = Searcher->GetRequestData();
	if (ShouldCleanList)
	{
		ShouldCleanList = false;
		ItemsSource.Reset();
	}

	if (ItemsSource.Num() > 0 && !ItemsSource.Last()->IsSet())
	{
		ItemsSource.Pop(false);
	}
	for (RequiredType& Str : Result.Value)
	{
		ItemsSource.Add(MakeShared<TOptional<RequiredType>>(Str));
	}
	if (!Result.Key)
	{
		ItemsSource.Add(MakeShared<TOptional<RequiredType>>());
	}
	ListView->RebuildList();
}

bool SSearchEverywhereWidget::SupportsKeyboardFocus() const
{
	return true;
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
