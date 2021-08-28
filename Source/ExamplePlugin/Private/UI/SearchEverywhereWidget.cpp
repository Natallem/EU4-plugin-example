#include "SearchEverywhereWidget.h"

#include "EditorStyleSet.h"
#include "Framework/Application/SlateApplication.h"
#include "Multithreading/Searcher.h"
#include "Multithreading/SearchableItem.h"
#include "SettingsData/Details/AbstractSettingDetail.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/SWindow.h"
#include "SearchEverywhereWindow.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void SSearchEverywhereWidget::Construct(const FArguments& InArgs, TSharedRef<SSearchEverywhereWindow> InParentWindow)
{
	ParentWindow = InParentWindow;
	Searcher = ParentWindow->GetSearcher();

	auto CreateFilterButton = [this](ESettingType Type, const FText& Name) -> TSharedRef<SButton>
	{
		return SNew(SButton)
				.Text(Name)
				.ForegroundColor(FCoreStyle::Get().GetSlateColor("DefaultForeground"))
				.ButtonColorAndOpacity(this, &SSearchEverywhereWidget::GetButtonColor, Type)
				.OnClicked(this, &SSearchEverywhereWidget::ChangeSettingSearchType, Type);
	};

	const TSharedRef<SWidget> TabsWidget = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			CreateFilterButton(All, FText::FromString("All"))
		] + SHorizontalBox::Slot()
		.AutoWidth()
		[
			CreateFilterButton(Category, FText::FromString("Category"))
		] + SHorizontalBox::Slot()
		.AutoWidth()
		[
			CreateFilterButton(Section, FText::FromString("Section"))
		] + SHorizontalBox::Slot()
		.AutoWidth()
		[
			CreateFilterButton(InnerCategory, FText::FromString("Inner Category"))
		] + SHorizontalBox::Slot()
		.AutoWidth()

		[
			CreateFilterButton(Property, FText::FromString("Property"))
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
		0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SSearchEverywhereWidget::SetKeyboardFocus));

	ItemsListView = SNew(SListViewWidget)
		.ItemHeight(64)
		.ListItemsSource(&ItemsSource)
		.SelectionMode(ESelectionMode::Single)
		.OnGenerateRow(this, &SSearchEverywhereWidget::OnGenerateTabSwitchListItemWidget)
		.OnSelectionChanged(this, &SSearchEverywhereWidget::OnListSelectionChanged)
		.OnMouseButtonClick(this, &SSearchEverywhereWidget::OnSelectItem);

	const TSharedRef<SWidget> ListTableWidget = SNew(SVerticalBox)
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
					ListTableWidget
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

void SSearchEverywhereWidget::ProcessMessage(const FItemFoundMsg& InMessage)
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
	if (InMessage.Storage.IsType<TArray<TSharedRef<ISearchableItem>>>())
	{
		for (const TSharedRef<ISearchableItem>& Str : InMessage.Storage.Get<TArray<TSharedRef<ISearchableItem>>>())
		{
			ItemsSource.Add(MakeShared<TOptional<TSharedRef<ISearchableItem>>>(Str));
		}
	}
	else if (InMessage.Storage.IsType<TSharedRef<ISearchableItem>>())
	{
		ItemsSource.Add(
			MakeShared<TOptional<TSharedRef<ISearchableItem>>>(InMessage.Storage.Get<TSharedRef<ISearchableItem>>()));
	}
	if (!InMessage.bIsFinished)
	{
		ItemsSource.Add(MakeShared<TOptional<TSharedRef<ISearchableItem>>>());
	}
	ItemsListView->RebuildList();
	CycleSelection();
}

EActiveTimerReturnType SSearchEverywhereWidget::SetKeyboardFocus(double InCurrentTime, float InDeltaTime) const
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
	ItemsListView->ClearSelection();
	Searcher->SetInput(Filter.ToString(), SearchSettingType);
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
				? InItem->GetValue()->GetRowWidget(SearchEditableText->GetText())
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
		InItem->GetValue()->DoAction();
		ParentWindow->RequestDestroyWindow();
	}
	else
	{
		Searcher->FindMoreDataResult();
		SetKeyboardFocus(0.0, 0.f);
	}
}

FReply SSearchEverywhereWidget::ChangeSettingSearchType(ESettingType NewType)
{
	if (NewType != SearchSettingType)
	{
		bShouldCleanList = true;
		SearchSettingType = NewType;
		ItemsListView->ClearSelection();
		Searcher->SetInput(SearchEditableText->GetText().ToString(), SearchSettingType);
	}
	SetKeyboardFocus(0.0, 0.f);
	return FReply::Handled();
}

FSlateColor SSearchEverywhereWidget::GetButtonColor(ESettingType ButtonType) const
{
	if (SearchSettingType == ButtonType)
	{
		return FLinearColor(1.0, 1.0, 0.0, 0.0);
	}
	static constexpr float Percentage = 0.24;
	return FLinearColor(Percentage, Percentage, Percentage, Percentage);
}

#undef LOCTEXT_NAMESPACE
