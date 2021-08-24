#include "SearchEverywhereWidget.h"

#include "EditorStyleSet.h"
#include "Framework/Application/SlateApplication.h"
#include "Multithreading/Searcher.h"
#include "SettingsData/Details/AbstractSettingDetail.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/SWindow.h"
#include "SearchEverywhereWindow.h"
#include "Widgets/Input/SButton.h"
#include "Multithreading/ResultItemFoundMsg.h"

#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void SSearchEverywhereWidget::Construct(const FArguments& InArgs, TSharedRef<SSearchEverywhereWindow> InParentWindow)
{
	ParentWindow = InParentWindow;
	Searcher = ParentWindow->GetSearcher();

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
			.OnClicked_Static(&SSearchEverywhereWidget::LogAllProperties)
		];

	const TSharedRef<SWidget> SearchTableWidget =
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .Padding(10.0f)
		  .FillWidth(1.0f)
		[
			SAssignNew(SearchEditableText, SEditableText)
			.Text(InArgs._PreviousSearchRequest)
			.HintText(LOCTEXT("SearchEditableTextHint", "Type here ..."))
			.OnKeyDownHandler(this, &SSearchEverywhereWidget::OnSearchTextKeyDown)
			.OnTextChanged(this, &SSearchEverywhereWidget::OnTextChanged)
			.OnTextCommitted(this, &SSearchEverywhereWidget::OnTextCommit)
			.SelectAllTextWhenFocused_Lambda([this]()-> bool
			{
				const bool ShouldSelectText = bIsFirstFocusActivate;
				if (bIsFirstFocusActivate)
				{
					if (!SearchEditableText->GetText().IsEmpty())
					{
						OnTextChanged(SearchEditableText->GetText());
					}
					bIsFirstFocusActivate = false;
				}
				return ShouldSelectText;
			})

		];

	RegisterActiveTimer(
		0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SSearchEverywhereWidget::SetFocusPostConstruct));

	ItemsListView = SNew(SListViewWidget)
		.ItemHeight(64)
		.ListItemsSource(&ItemsSource)
		.SelectionMode(ESelectionMode::Single)
		.OnGenerateRow(this, &SSearchEverywhereWidget::OnGenerateTabSwitchListItemWidget)
		.OnSelectionChanged(this, &SSearchEverywhereWidget::OnListSelectionChanged)
		.OnMouseButtonClick(this, &SSearchEverywhereWidget::OnSelectItem);

	ListTableWidget = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SScrollBorder, ItemsListView.ToSharedRef())
			[
				ItemsListView.ToSharedRef()
			]
		];
	SAssignNew(ShowMoreResultsItem, STextBlock)
		.Text(LOCTEXT("ShowMoreResultsItemText", "More ..."));

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
	CycleSelection();
}

bool SSearchEverywhereWidget::SupportsKeyboardFocus() const
{
	return true;
}

FText SSearchEverywhereWidget::GetCurrentSearchRequest() const
{
	return SearchEditableText->GetText();
}

void SSearchEverywhereWidget::ProcessMessage(const FResultItemFoundMsg& InMessage)
{
	if (bShouldCleanList)
	{
		bShouldCleanList = false;
		ItemsSource.Reset();
	}

	if (ItemsSource.Num() > 0 && !ItemsSource.Last()->IsSet())
	{
		ItemsSource.Pop(false);
	}
	if (InMessage.Storage.IsType<TArray<RequiredType>>())
	{
		for (const RequiredType& Str : InMessage.Storage.Get<TArray<RequiredType>>())
		{
			ItemsSource.Add(MakeShared<TOptional<RequiredType>>(Str));
		}
	}
	else if (InMessage.Storage.IsType<RequiredType>())
	{
		ItemsSource.Add(MakeShared<TOptional<RequiredType>>(InMessage.Storage.Get<RequiredType>()));
	}
	if (!InMessage.bIsFinished)
	{
		ItemsSource.Add(MakeShared<TOptional<RequiredType>>());
	}
	ItemsListView->RebuildList();
	CycleSelection();
}

EActiveTimerReturnType SSearchEverywhereWidget::SetFocusPostConstruct(double InCurrentTime, float InDeltaTime) const
{
	if (SearchEditableText.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(SearchEditableText.ToSharedRef(), EFocusCause::SetDirectly);
	}
	return EActiveTimerReturnType::Stop;
}

void SSearchEverywhereWidget::OnTextChanged(const FText& Filter)
{
	bShouldCleanList = true;
	Searcher->SetInput(Filter.ToString());
}

void SSearchEverywhereWidget::OnTextCommit(const FText& CommittedText, ETextCommit::Type CommitType) const
{
	if (CommitType == ETextCommit::OnEnter && ItemsListView->GetNumItemsSelected() != 0)
	{
		OnSelectItem(ItemsListView->GetSelectedItems()[0]);
	}
}

FReply SSearchEverywhereWidget::OnSearchTextKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	UE_LOG(LogTemp, Log, TEXT("EP : OnSearchTextKeyDown %s"), *InKeyEvent.GetKey().ToString());
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		ParentWindow->RequestDestroyWindow();
		return FReply::Handled();
	}
	if (InKeyEvent.GetKey() == EKeys::Up || InKeyEvent.GetKey() == EKeys::Down)
	{
		CycleSelection(true, InKeyEvent.GetKey() == EKeys::Down);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

TSharedRef<ITableRow> SSearchEverywhereWidget::OnGenerateTabSwitchListItemWidget(FListItemPtr InItem,
	const TSharedRef<STableViewBase>& OwnerTable) const
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			InItem->IsSet()
				? PropertyHolder.GetSettingDetail(InItem->GetValue())->GetRowWidget()
				: ShowMoreResultsItem.ToSharedRef()
		]
	];
}

void SSearchEverywhereWidget::OnListSelectionChanged(FListItemPtr InItem, ESelectInfo::Type SelectInfo)
{
	SelectedListViewItemIndex = ItemsSource.Find(InItem);
}

void SSearchEverywhereWidget::CycleSelection(bool bIsMoving, bool bIsDownMoving)
{
	if ((ItemsListView->GetNumItemsSelected() == 0) && (ItemsSource.Num() > 0))
	{
		SelectedListViewItemIndex = 0;
		ItemsListView->SetSelection(ItemsSource[SelectedListViewItemIndex]);
		return;
	}

	if (bIsMoving)
	{
		const int64 NewIndex = SelectedListViewItemIndex + (bIsDownMoving ? 1 : -1);
		if (NewIndex < 0 || NewIndex >= ItemsSource.Num())
		{
			return;
		}
		SelectedListViewItemIndex = NewIndex;
		const FListItemPtr& NewSelectedItem = ItemsSource[SelectedListViewItemIndex];
		ItemsListView->SetSelection(NewSelectedItem);
		ItemsListView->RequestScrollIntoView(NewSelectedItem);
	}
}

void SSearchEverywhereWidget::OnSelectItem(FListItemPtr InItem) const
{
	if (InItem->IsSet())
	{
		PropertyHolder.GetSettingDetail(InItem->GetValue())->DoAction();
		ParentWindow->RequestDestroyWindow();
	}
	else
	{
		Searcher->FindMoreDataResult();
		SetFocusPostConstruct(0.0, 0.f);
	}
}

FReply SSearchEverywhereWidget::OpenSettings(FName InContainerName, FName InCategoryName, FName InSectionName) const
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer(
		InContainerName, InCategoryName, InSectionName);
	return FReply::Handled();
}

FReply SSearchEverywhereWidget::LogAllProperties()
{
	FPropertyHolder::GetSettingsData<true>();
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
