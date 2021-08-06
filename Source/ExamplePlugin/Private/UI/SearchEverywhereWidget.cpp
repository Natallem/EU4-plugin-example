#include "SearchEverywhereWidget.h"

#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/STextEntryPopup.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "STextPropertyEditableTextBox.h"
#include "EditorStyleSet.h"
#include "IDetailTreeNode.h"
#include "IPropertyRowGenerator.h"
#include "ISettingsCategory.h"
#include "ISettingsContainer.h"
#include "Multithreading/Searcher.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"


#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void SSearchEverywhereWidget::Construct(const FArguments& InArgs, TSharedRef<FSearcher> SearcherArgument)
{
	Searcher = SearcherArgument;
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
			.OnClicked(this, &SSearchEverywhereWidget::OpenSettings, FName("Editor"), FName("General"),
			           FName("Appearance"))

		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
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
					ListTableWidget
				]
			]
		]
	];
}

void SSearchEverywhereWidget::UpdateShownResults()
{
	TPair<bool, TArray<FString>> Result = Searcher->GetRequestData();
	if (ShouldCleanList)
	{
		ShouldCleanList = false;
		StringItems.Reset();
	}

	if (StringItems.Num() > 0 && !StringItems.Last()->IsSet())
	{
		StringItems.Pop(false);
	}
	for (FString& Str : Result.Value)
	{
		StringItems.Add(MakeShared<TOptional<FString>>(Str));
	}
	if (!Result.Key)
	{
		StringItems.Add(MakeShared<TOptional<FString>>());
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


FReply SSearchEverywhereWidget::OpenSettings(FName ContainerName, FName CategoryName, FName SectionName)
{
	TArray<FName> OutNames;
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
	SettingsModule.GetContainerNames(OutNames);
	TSharedPtr<ISettingsContainer> EditorSettingContainer = SettingsModule.GetContainer("Editor");
	TArray<TSharedPtr<ISettingsCategory>> EditorSettingContainerCategories;
	EditorSettingContainer->GetCategories(EditorSettingContainerCategories);
	for (TSharedPtr<ISettingsCategory> Category : EditorSettingContainerCategories)
	{
		const FName& CategoryName = Category->GetName();

		const FText& CategoryDescription = Category->GetDescription();
		TArray<TSharedPtr<ISettingsSection>> Sections;
		Category->GetSections(Sections);
		const FText& CategoryDisplayName = Category->GetDisplayName();

		for (TSharedPtr<ISettingsSection> Section : Sections)
		{
			const FName& SectionName = Section->GetName();
			const FText& SectionDisplayName = Section->GetDisplayName();
			const FText& SectionDescription = Section->GetDescription();

			FText SectionStatus = Section->GetStatus();
			const FName& SectionCategoryName = Section->GetCategory().Pin()->GetName();
			TWeakObjectPtr<UObject> SectionObject = Section->GetSettingsObject();
			UObject* t = SectionObject.Get();
			FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
				"PropertyEditor");
			FPropertyRowGeneratorArgs Args;
			Args.bAllowMultipleTopLevelObjects = true;
			TSharedPtr<IPropertyRowGenerator> Generator = PropertyEditorModule.CreatePropertyRowGenerator(Args);
			TArray<UObject*> Objects;
			Objects.Add(t);
			Generator->SetObjects(Objects);
			const TArray<TSharedRef<IDetailTreeNode>>& DetailTreeNodes = Generator->GetRootTreeNodes();
			UE_LOG(LogTemp, Log, TEXT("EP : Category '%s' section '%s'"), *CategoryDisplayName.ToString(),
            			*SectionDisplayName.ToString());
			for (const TSharedRef<IDetailTreeNode>& Ref : DetailTreeNodes)
			{
				ProcessIDetailTreeNode(Ref);
			}
			
		}
	}
	SettingsModule.ShowViewer(ContainerName, CategoryName, SectionName);
	return FReply::Handled();
}

void SSearchEverywhereWidget::ProcessIDetailTreeNode(const TSharedRef<IDetailTreeNode>& Node)
{
	TSharedPtr<IPropertyHandle> Handle = Node->CreatePropertyHandle();
	EDetailNodeType type = Node->GetNodeType();
	if (Handle == nullptr && type != EDetailNodeType::Advanced && type != EDetailNodeType::Category)
	{
		TSharedPtr<IPropertyHandle> Handle2 = Node->CreatePropertyHandle();
		int x = 10;
	}


	TArray<TSharedRef<IDetailTreeNode>> Childrens;

	switch (type)
	{
	case EDetailNodeType::Category:
		{
			Node->GetChildren(Childrens);
			for (TSharedRef<IDetailTreeNode> Child : Childrens)
			{
				ProcessIDetailTreeNode(Child);
			}
			FText Name;
			 FName NameCategory;
			FName NameNode = Node->GetNodeName();
			if (Handle)
			{
				NameCategory = Handle->GetDefaultCategoryName();
				Name = Handle->GetPropertyDisplayName();
			}
			UE_LOG(LogTemp, Log, TEXT("EP : Category Name '%s', number children = %d"), *Name.ToString(), Childrens.Num());
			int x = 10;

			break;
		}
	case EDetailNodeType::Item:
		{
			FText Name;
			if (Handle)
			{
				Name = Handle->GetPropertyDisplayName();
			}
			Node->GetChildren(Childrens);
			UE_LOG(LogTemp, Log, TEXT("EP : Item Name '%s', number children = %d"), *Name.ToString(), Childrens.Num());
			int x = 10;

			break;
		}
	case EDetailNodeType::Advanced:
		{
			int x = 10;
			break;
		}
	case EDetailNodeType::Object:
		{
			int x = 10;
			break;
		}
	}
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
		InnerWidget = SNew(STextBlock)
			.Text(FText::FromString(*InItem->GetValue()));
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


#undef LOCTEXT_NAMESPACE
