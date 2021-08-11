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
#include "Classes/EditorStyleSettings.h"
#include "PropertyEditor/Private/DetailItemNode.h"
#include "PropertyEditor/Private/SDetailsView.h"

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
			.OnClicked(this, &SSearchEverywhereWidget::FirstAttemptToGetAllProperties)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("Button2Text", "Button2"))
			.OnClicked(this, &SSearchEverywhereWidget::SecondAttemptToGetAllProperties)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("Button3Text", "Button3"))
			.OnClicked(this, &SSearchEverywhereWidget::ThirdAttemptToGetAllProperties)
			// .OnClicked(this, &SSearchEverywhereWidget::OpenSettings, FName("Editor"), FName("General"),
			// FName("Appearance"))
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

FReply SSearchEverywhereWidget::OpenSettings(FName InContainerName, FName InCategoryName, FName InSectionName)
{
	SettingsModule.ShowViewer(InContainerName, InCategoryName, InSectionName);
	return FReply::Handled();
}

void SSearchEverywhereWidget::WriteLog(const FString& Text, int LogNumber, bool IsAppend)
{
	FString FileLog =
		TEXT(R"(C:\Projects\UnrealEngineProjects\ExampleProject\ExampleProject\Plugins\ExamplePlugin\Resources\Log_)") +
		FString::FromInt(LogNumber) + ".txt";
	FFileHelper::SaveStringToFile(Text, *FileLog, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(),
	                              (IsAppend) ? FILEWRITE_Append : FILEWRITE_None);
}

FReply SSearchEverywhereWidget::FirstAttemptToGetAllProperties()
{
	WriteLog("FirstAttemptToGetAllProperties\n", 1, false);
	TArray<FName> OutNames;
	SettingsModule.GetContainerNames(OutNames);
	TSharedPtr<ISettingsContainer> EditorSettingContainer = SettingsModule.GetContainer("Editor");
	TArray<TSharedPtr<ISettingsCategory>> EditorSettingContainerCategories;
	EditorSettingContainer->GetCategories(EditorSettingContainerCategories);
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
		"PropertyEditor");
	FPropertyRowGeneratorArgs Args;
	Args.bAllowMultipleTopLevelObjects = true;
	Args.bShouldShowHiddenProperties = true;

	for (const TSharedPtr<ISettingsCategory>& Category : EditorSettingContainerCategories)
	{
		FName CategoryName = Category->GetName();
		const FText& CategoryDescription = Category->GetDescription();
		const FText& CategoryDisplayName = Category->GetDisplayName();
		WriteLog(FString::Printf(
			         TEXT("CategoryName: '%s', CategoryDisplayName: '%s', CategoryDescription: '%s'\n"),
			         *CategoryName.ToString(),
			         *CategoryDisplayName.ToString(), *CategoryDescription.ToString()), 1);

		TArray<TSharedPtr<ISettingsSection>> Sections;
		Category->GetSections(Sections);

		for (const TSharedPtr<ISettingsSection>& Section : Sections)
		{
			const FName& SectionName = Section->GetName();
			const FText& SectionDisplayName = Section->GetDisplayName();
			const FText& SectionDescription = Section->GetDescription();
			WriteLog(FString::Printf(
				         TEXT("	SectionName: '%s', SectionDisplayName: '%s', SectionDescription: '%s'\n"),
				         *SectionName.ToString(),
				         *SectionDisplayName.ToString(), *SectionDescription.ToString()), 1);

			FText SectionStatus = Section->GetStatus();
			const FName& SectionCategoryName = Section->GetCategory().Pin()->GetName();
			TWeakObjectPtr<UObject> SectionObject = Section->GetSettingsObject();

			TSharedPtr<IPropertyRowGenerator> Generator = PropertyEditorModule.CreatePropertyRowGenerator(Args);
			TArray<UObject*> Objects;
			UObject* obj = SectionObject.Get();
			Objects.Add(SectionObject.Get());
			Generator->SetObjects(Objects);
			TArray<TSharedRef<IDetailTreeNode>> DetailTreeNodes = Generator->GetRootTreeNodes();
			for (TSharedRef<IDetailTreeNode>& Ref : DetailTreeNodes)
			{
				ProcessIDetailTreeNode(Ref, "		");
			}
		}
	}
	return FReply::Handled();
}

void SSearchEverywhereWidget::ProcessIDetailTreeNode(TSharedRef<IDetailTreeNode>& Node, const FString& Tabs)
{
	TSharedPtr<IPropertyHandle> Handle = Node->CreatePropertyHandle();
	const EDetailNodeType Type = Node->GetNodeType();
	FName NameNode = Node->GetNodeName();
	static TMap<EDetailNodeType, FString> TypeMap =
	{
		{EDetailNodeType::Item, "Item"},
		{EDetailNodeType::Advanced, "Advanced"},
		{EDetailNodeType::Category, "Category"},
		{EDetailNodeType::Object, "Object"},
	};
	WriteLog(Tabs + "Node type: \'" + TypeMap[Type] + "\', has " + (Handle ? "" : "no ") + "Handle", 1);

	TArray<TSharedRef<IDetailTreeNode>> Childrens;

	switch (Type)
	{
	case EDetailNodeType::Category:
		{
			Node->GetChildren(Childrens);
			FText PropertyDisplayName;
			FName DefaultCategoryName;
			const FName NodeName = Node->GetNodeName();
			if (Handle)
			{
				DefaultCategoryName = Handle->GetDefaultCategoryName();
				PropertyDisplayName = Handle->GetPropertyDisplayName();
			}
			WriteLog(", children count = " + FString::FromInt(Childrens.Num()), 1);
			if (!PropertyDisplayName.IsEmpty())
			{
				WriteLog(", PropertyDisplayName: \'" + PropertyDisplayName.ToString() + '\'', 1);
			}
			if (!DefaultCategoryName.IsNone())
			{
				WriteLog(", DefaultCategoryName: \'" + DefaultCategoryName.ToString() + '\'', 1);
			}
			if (!NodeName.IsNone())
			{
				if (NodeName == "Accessor")
				{
					ListTableWidget = Childrens[0]->CreateNodeWidgets().WholeRowWidget;
					// UObject* obj = Childrens[0];
					TSharedPtr<IDetailPropertyRow> t = Childrens[0]->GetRow();
					FName T2 = Childrens[0]->GetNodeName();
					// WriteLog(", NameNode: \'" + NodeName.ToString() + '\'');
					// break;
				}
				WriteLog(", NameNode: \'" + NodeName.ToString() + '\'', 1);
			}
			if (Childrens.Num() != 0)
			{
				WriteLog("\n", 1);
				for (TSharedRef<IDetailTreeNode> Child : Childrens)
				{
					ProcessIDetailTreeNode(Child, Tabs + "	");
				}
			}
			break;
		}
	case EDetailNodeType::Item:
		{
			FText PropertyDisplayName;

			if (Handle)
			{
				PropertyDisplayName = Handle->GetPropertyDisplayName();
			}
			Node->GetChildren(Childrens);
			WriteLog(FString::Printf(
				         TEXT(", PropertyDisplayName: '%s', children count = %d"), *PropertyDisplayName.ToString(),
				         Childrens.Num()), 1);
			if (Childrens.Num() == 0 && PropertyDisplayName.IsEmpty())
			{
				WriteLog(FString::Printf(TEXT(" Hidden")), 1);
			}
			if (Childrens.Num() != 0)
			{
				WriteLog("\n", 1);
				for (TSharedRef<IDetailTreeNode> Child : Childrens)
				{
					ProcessIDetailTreeNode(Child, Tabs + "	");
				}
			}

			if (PropertyDisplayName.IsEmpty() && Childrens.Num() == 0 && Handle == nullptr)
			{
				TSharedRef<FDetailItemNode> ItemNode = StaticCastSharedRef<FDetailItemNode>(Node);
				// Handle = NodeRow->GetPropertyHandle();
				int x = 10;
			}
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
	WriteLog("\n", 1);
}

FReply SSearchEverywhereWidget::SecondAttemptToGetAllProperties()
{
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bShowDifferingPropertiesOption = true;
	DetailsViewArgs.bShowModifiedPropertiesOption = true;
	DetailsViewArgs.bForceHiddenPropertyVisibility = true;
	DetailsViewArgs.bShowCustomFilterOption = true;

	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	TSharedRef<SDetailsView> DetailsView = StaticCastSharedRef<SDetailsView>(
		EditModule.CreateDetailView(DetailsViewArgs));
	FDetailsViewArgs args;
	DetailsView->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateStatic([] { return false; }));
	DetailsView->ShowAllAdvancedProperties();
	DetailsView->SetOnDisplayedPropertiesChanged(
		::FOnDisplayedPropertiesChanged::CreateRaw(this, &SSearchEverywhereWidget::HandlePropertiesChanged));

	WriteLog("SecondAttemptToGetAllProperties\n", 2, false);

	TSharedPtr<ISettingsContainer> EditorSettingContainer = SettingsModule.GetContainer("Editor");
	TArray<TSharedPtr<ISettingsCategory>> EditorSettingContainerCategories;
	EditorSettingContainer->GetCategories(EditorSettingContainerCategories);
	for (const TSharedPtr<ISettingsCategory>& Category : EditorSettingContainerCategories)
	{
		FName CategoryName = Category->GetName();
		const FText& CategoryDescription = Category->GetDescription();
		const FText& CategoryDisplayName = Category->GetDisplayName();
		WriteLog(FString::Printf(
			         TEXT("CategoryName: '%s', CategoryDisplayName: '%s', CategoryDescription: '%s'\n"),
			         *CategoryName.ToString(),
			         *CategoryDisplayName.ToString(), *CategoryDescription.ToString()), 2);

		TArray<TSharedPtr<ISettingsSection>> Sections;
		Category->GetSections(Sections);
		for (const TSharedPtr<ISettingsSection>& Section : Sections)
		{
			const FName& SectionName = Section->GetName();
			const FText& SectionDisplayName = Section->GetDisplayName();
			const FText& SectionDescription = Section->GetDescription();
			WriteLog(FString::Printf(
				         TEXT("	SectionName: '%s', SectionDisplayName: '%s', SectionDescription: '%s'\n"),
				         *SectionName.ToString(),
				         *SectionDisplayName.ToString(), *SectionDescription.ToString()), 2);
			if (SectionName.ToString() == "Source Code")
			{
				TWeakPtr<SWidget> ptr = Section->GetCustomWidget();
				ListTableWidget = Section->GetCustomWidget().Pin();
			}
			FText PropertyDisplayName;
			TSet<FString> PropValues;
			TSet<FString> PathValues;
			TMap<FString, FProperty*> PropertyMap;

			UObject* SectionObject = Section->GetSettingsObject().Get();
			FString SectionObjectName = SectionObject->GetName();

			WriteLog(FString::Printf(TEXT("		SectionObjectName: '%s'\n"), *SectionObjectName), 2);
			for (TFieldIterator<FProperty> PropertyIt(SectionObject->GetClass()); PropertyIt; ++PropertyIt)
			{
				// if (!PropertyIt->HasMetaData(TEXT("config")))
				// {
					// continue;
				// }
				FString CppName = PropertyIt->GetNameCPP();
				PropValues.Add(CppName);
				PropertyMap.FindOrAdd(CppName) = *PropertyIt;
			}
			DetailsView->SetObject(SectionObject);
			TArray<FPropertyPath> paths = DetailsView->GetPropertiesInOrderDisplayed();
			for (FPropertyPath& path : paths)
			{
				PathValues.Add(path.ToString());
				// WriteLog(FString::Printf(TEXT("		Path: '%s'\n"), *path.ToString()), 2);
			}
			TSet<FString> Intersection = PropValues.Intersect(PathValues);
			TMap<FString, TArray<FProperty*>> CategoryPropertyMap;
			for (FString& PropertyName : Intersection)
			{
				const FString& CategoryPropertyName = PropertyMap[PropertyName]->GetMetaData(TEXT("Category"));
				CategoryPropertyMap.FindOrAdd(CategoryPropertyName).Add(PropertyMap[PropertyName]);
			}
			for (const TPair<FString, TArray<FProperty*>>& Pair : CategoryPropertyMap)
			{
				WriteLog(FString::Printf(TEXT("		CategoryPropertyName: '%s'\n"), *Pair.Key), 2);
				for (FProperty* Property : Pair.Value)
				{
					WriteLog(FString::Printf(
						         TEXT("			Both: '%s', Display name: '%s'\n"), *Property->GetNameCPP(),
						         *Property->GetDisplayNameText().ToString()), 2);
				}
			}
			
			TSet<FString> InPropNotInPath = PropValues.Difference(PathValues);
			TSet<FString> InPathNotInProp = PathValues.Difference(PropValues);
			if (InPathNotInProp.Num() != 0)
			{
				WriteLog(FString::Printf(
					         TEXT("		Difference: in Paths, not in Properties(Category: '%s', Section: '%s'):\n"),
					         *CategoryDisplayName.ToString(), *SectionDisplayName.ToString()), 2);
				for (FString& Diff : InPathNotInProp)
				{
					WriteLog("			\'" + Diff + "\'\n", 2);
				}
			}
			if (InPropNotInPath.Num() != 0)
			{
				WriteLog(FString::Printf(
					         TEXT("		Difference: in Properties, not in Paths(Category: '%s', Section: '%s'):\n"),
					         *CategoryDisplayName.ToString(), *SectionDisplayName.ToString()), 2);
				for (FString& Diff : InPropNotInPath)
				{
					FString CategoryPropertyName = PropertyMap[Diff]->GetMetaData(TEXT("Category"));

					WriteLog(FString::Printf(
						         TEXT("				'%s', Display name: '%s', PropertyCategory: '%s'\n"), *Diff,
						         *PropertyMap[Diff]->GetDisplayNameText().ToString(), *CategoryPropertyName), 2);
				}
			}
		}
	}

	return FReply::Handled();
}

FReply SSearchEverywhereWidget::ThirdAttemptToGetAllProperties()
{
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* UClassPtr = *It;
		for (TFieldIterator<FProperty> Prop(UClassPtr); Prop; ++Prop)
		{
			WriteLog(Prop->GetNameCPP() + "\n", 3);
			Prop->GetMetaDataMap();
		}
	}
	return FReply::Handled();
}

void SSearchEverywhereWidget::HandlePropertiesChanged()
{
}


#undef LOCTEXT_NAMESPACE
