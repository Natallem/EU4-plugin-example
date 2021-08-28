#include "PropertyHolder.h"

#include "Details/CategoryDetail.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "Misc/FileHelper.h"
#include "IDetailTreeNode.h"
#include "Details/AbstractSettingDetail.h"
#include "Details/InnerCategoryDetail.h"
#include "Details/PropertyDetail.h"
#include "Details/SectionDetail.h"
#include "Widgets/Docking/SDockTab.h"

#include "SettingsEditor/Private/Widgets/SSettingsEditor.h"
#include "SettingsEditor/Private/Widgets/SSettingsSectionHeader.h"
#include "PropertyEditor/Private/SDetailsView.h"
#include "PropertyEditor/Private/DetailLayoutBuilderImpl.h"
#include "PropertyEditor/Private/PropertyRowGenerator.h"
#include "PropertyEditor/Private/DetailMultiTopLevelObjectRootNode.h"
#include "PropertyEditor/Private/DetailCategoryBuilderImpl.h"
#include "PropertyEditor/Private/DetailGroup.h"
#include "PropertyEditor/Private/DetailItemNode.h"
#include "Multithreading/InputData.h"

DEFINE_LOG_CATEGORY(LogPropertyHolder);

/** For more information see http://bloglitb.blogspot.com/2011/12/access-to-private-members-safer.html */
namespace PrivateFieldHack
{
	template <typename Tag, typename Tag::type M>
	struct THelperPrivateField
	{
		friend typename Tag::type GetPrivate(Tag)
		{
			return M;
		}
	};

	struct FGetter_SettingsView_From_SSettingsEditor
	{
		typedef TSharedPtr<IDetailsView> SSettingsEditor::* type;
		friend type GetPrivate(FGetter_SettingsView_From_SSettingsEditor);
	};

	template struct THelperPrivateField<FGetter_SettingsView_From_SSettingsEditor, &SSettingsEditor::SettingsView>;

	struct FGetter_Model_From_FSettingsDetailRootObjectCustomization
	{
		typedef ISettingsEditorModelPtr FSettingsDetailRootObjectCustomization::* type;
		friend type GetPrivate(FGetter_Model_From_FSettingsDetailRootObjectCustomization);
	};

	template struct THelperPrivateField<FGetter_Model_From_FSettingsDetailRootObjectCustomization,
	                                    &FSettingsDetailRootObjectCustomization::Model>;

	struct FGetter_DetailLayouts_From_SDetailsViewBase
	{
		typedef FDetailLayoutList SDetailsViewBase::* type;
		friend type GetPrivate(FGetter_DetailLayouts_From_SDetailsViewBase);
	};

	template struct THelperPrivateField<FGetter_DetailLayouts_From_SDetailsViewBase, &SDetailsViewBase::DetailLayouts>;

	struct FGetter_RootObjectSet_From_FDetailMultiTopLevelObjectRootNode
	{
		typedef FDetailsObjectSet FDetailMultiTopLevelObjectRootNode::* type;
		friend type GetPrivate(FGetter_RootObjectSet_From_FDetailMultiTopLevelObjectRootNode);
	};

	template struct THelperPrivateField<FGetter_RootObjectSet_From_FDetailMultiTopLevelObjectRootNode,
	                                    &FDetailMultiTopLevelObjectRootNode::RootObjectSet>;

	struct FGetter_Customization_From_FDetailItemNode
	{
		typedef FDetailLayoutCustomization FDetailItemNode::* type;
		friend type GetPrivate(FGetter_Customization_From_FDetailItemNode);
	};

	template struct THelperPrivateField<FGetter_Customization_From_FDetailItemNode, &FDetailItemNode::Customization>;

	struct FGetter_LocalizedDisplayName_From_FDetailGroup
	{
		typedef FText FDetailGroup::* type;
		friend type GetPrivate(FGetter_LocalizedDisplayName_From_FDetailGroup);
	};

	template struct THelperPrivateField<FGetter_LocalizedDisplayName_From_FDetailGroup, &
	                                    FDetailGroup::LocalizedDisplayName>;
}

FSettingsDataCollection::FSettingsDataCollection(const TArray<TSharedRef<FCategoryDetail>>& CategoryDetails,
                                                 const TArray<TSharedRef<FSectionDetail>>& SectionDetails,
                                                 const TArray<TSharedRef<FInnerCategoryDetail>>& InnerCategoryDetails,
                                                 const TArray<TSharedRef<FPropertyDetail>>& PropertyDetails):
	CategoryDetails(CategoryDetails),
	SectionDetails(SectionDetails),
	InnerCategoryDetails(InnerCategoryDetails),
	PropertyDetails(PropertyDetails)
{
}

int FSettingsDataCollection::Num() const
{
	switch (SettingType)
	{
	case All:
		return CategoryDetails.Num() + SectionDetails.Num() + InnerCategoryDetails.Num() + PropertyDetails.Num();
	case Category:
		return CategoryDetails.Num();
	case Section:
		return SectionDetails.Num();
	case InnerCategory:
		return InnerCategoryDetails.Num();
	case Property:
		return PropertyDetails.Num();
	}
	return 0;
}

TSharedRef<FAbstractSettingDetail> FSettingsDataCollection::operator[](int Index) const
{
	switch (SettingType)
	{
	case Category:
		return CategoryDetails[Index];
	case Section:
		return SectionDetails[Index];
	case InnerCategory:
		return InnerCategoryDetails[Index];
	case Property:
		return PropertyDetails[Index];
	}
	int ShiftIndex = 0;
	if (ShiftIndex + CategoryDetails.Num() > Index)
	{
		return CategoryDetails[Index - ShiftIndex];
	}
	ShiftIndex += CategoryDetails.Num();

	if (ShiftIndex + SectionDetails.Num() > Index)
	{
		return SectionDetails[Index - ShiftIndex];
	}
	ShiftIndex += SectionDetails.Num();

	if (ShiftIndex + InnerCategoryDetails.Num() > Index)
	{
		return InnerCategoryDetails[Index - ShiftIndex];
	}
	ShiftIndex += InnerCategoryDetails.Num();

	return PropertyDetails[Index - ShiftIndex];
}


FPropertyHolder& FPropertyHolder::Get()
{
	static FPropertyHolder Holder;
	return Holder;
}

TOptional<TSharedRef<ISearchableItem>> FPropertyHolder::FindNextItem(
	const TSharedPtr<FInputData, ESPMode::ThreadSafe>& InputData)
{
	Data.SettingType = InputData->SearchType;
	static const int IterationBeforeCheck = 100; // Parameter
	int IterationCounter = 0;
	for (int i = InputData->NextIndexToCheck; i < Data.Num(); ++i)
	{
		++IterationCounter;
		if (IterationCounter == IterationBeforeCheck)
		{
			if (InputData->bIsCancelled)
			{
				return TOptional<TSharedRef<ISearchableItem>>();
			}
			IterationCounter = 0;
		}
		if (Data[i]->GetDisplayName().ToString().Find(*InputData->InputRequest) != -1)
		{
			InputData->NextIndexToCheck = i + 1;
			return TOptional<TSharedRef<ISearchableItem>>(Data[i]);
		}
	}
	InputData->NextIndexToCheck = Data.Num();
	return TOptional<TSharedRef<ISearchableItem>>();
}

bool FPropertyHolder::UpdateTreeNodes(
	const TSharedRef<FPropertyDetail> PropertyDetail, TSharedRef<FInnerCategoryDetail> InnerCategoryDetail)
{
	bool bIsFoundPropertyNode = false;
	FSettingsDataCollection Collection = GetSettingsData<false>();
	for (const TSharedRef<FPropertyDetail>& CurrentPropertyDetail : Collection.PropertyDetails)
	{
		if (CurrentPropertyDetail->GetDisplayName().EqualTo(PropertyDetail->GetDisplayName()))
		{
			if (CurrentPropertyDetail->PropertyDetailTreeNode.IsValid())
			{
				PropertyDetail->PropertyDetailTreeNode = CurrentPropertyDetail->PropertyDetailTreeNode.Pin();
				bIsFoundPropertyNode = true;
				break;
			}
			UE_LOG(LogSearchSettingDetail, Warning, TEXT("Cannot find node for property '%s'"),
			       *PropertyDetail->GetDisplayName().ToString())
			return false;
		}
	}
	if (!bIsFoundPropertyNode)
	{
		return false;
	}
	for (const TSharedRef<FInnerCategoryDetail>& CurrentInnerCategoryDetail : Collection.InnerCategoryDetails)
	{
		if (CurrentInnerCategoryDetail->GetDisplayName().EqualTo(InnerCategoryDetail->GetDisplayName()))
		{
			if (CurrentInnerCategoryDetail->CategoryTreeNode.IsValid())
			{
				InnerCategoryDetail->CategoryTreeNode = CurrentInnerCategoryDetail->CategoryTreeNode;
				return true;
			}
			UE_LOG(LogSearchSettingDetail, Warning, TEXT("Cannot find category node '%s'"),
			       *InnerCategoryDetail->GetDisplayName().ToString())
			return false;
		}
	}
	return false;;
}

template <bool bShouldLog>
FSettingsDataCollection FPropertyHolder::GetSettingsData(ISettingsModule& SettingModule)
{
	UE_LOG(LogPropertyHolder, Log, TEXT("Loading properties..."))
	if constexpr (bShouldLog)
	{
		WriteLog("Log properties\n\n", false);
	}

	FSettingsDataCollection Result;
	//Used to not duplicate Categories in Result
	TMap<TSharedPtr<ISettingsCategory>, TSharedPtr<FCategoryDetail>> FoundCategories;

	TSharedPtr<SDockTab> EditorSettingsTab;
	bool bShouldCloseSettings;
	const TSharedPtr<SDetailsView> DetailsView = GetDetailsView(EditorSettingsTab, bShouldCloseSettings, SettingModule);
	if (!DetailsView.IsValid())
	{
		return Result;
	}
	// get Model to find Section by object
	const TSharedPtr<FSettingsDetailRootObjectCustomization> DetailRootObjectCustomization = StaticCastSharedPtr<
		FSettingsDetailRootObjectCustomization>(DetailsView->GetRootObjectCustomization());
	const ISettingsEditorModelPtr Model = (*DetailRootObjectCustomization).*GetPrivate(
		PrivateFieldHack::FGetter_Model_From_FSettingsDetailRootObjectCustomization());

	FDetailLayoutList& DetailLayouts =
		(*StaticCastSharedPtr<SDetailsViewBase>(DetailsView)).*
		GetPrivate(PrivateFieldHack::FGetter_DetailLayouts_From_SDetailsViewBase());

	for (int DetailLayoutIndex = 0; DetailLayoutIndex < DetailLayouts.Num(); ++ DetailLayoutIndex)
	{
		const FDetailLayoutData& DetailLayout = DetailLayouts[DetailLayoutIndex];
		FDetailNodeList& AllRoots = DetailLayout.DetailLayout->GetAllRootTreeNodes();
		if (AllRoots.Num() == 0)
		{
			UE_LOG(LogPropertyHolder, Warning, TEXT("Property Holder got zero root Tree Node from DetailLayouts"))
			continue;
		}
		if (AllRoots.Num() > 1)
		{
			UE_LOG(LogPropertyHolder, Warning,
			       TEXT(
				       "Property Holder got %d RootTreeNodes from DetailLayouts. Process only first. Others will not be processed"
			       ),
			       AllRoots.Num())
		}
		TSharedRef<FDetailMultiTopLevelObjectRootNode> TopLevelNode =
			StaticCastSharedRef<FDetailMultiTopLevelObjectRootNode>(AllRoots[0]);
		TSharedPtr<FSectionDetail> SectionDetail = FindSectionByObjectAndCheck<bShouldLog>(
			TopLevelNode, FoundCategories, Result, SettingModule, Model, DetailLayoutIndex);
		if (SectionDetail.IsValid())
		{
			FindAllInnerCategoryDetails<bShouldLog>(TopLevelNode, SectionDetail.ToSharedRef(), Result);
		}
	}
	if (bShouldCloseSettings)
	{
		EditorSettingsTab->RequestCloseTab();
	}
	return Result;
}

/* Used to leave the definition of the template function in the .cpp file */
template FSettingsDataCollection FPropertyHolder::GetSettingsData<true>(ISettingsModule& SettingModule);
template FSettingsDataCollection FPropertyHolder::GetSettingsData<false>(ISettingsModule& SettingModule);

ISettingsModule& FPropertyHolder::GetSettingModule()
{
	return FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
}

FPropertyHolder::FPropertyHolder()
{
	Data = GetSettingsData<false>();
}

TSharedPtr<SDetailsView> FPropertyHolder::GetDetailsView(TSharedPtr<SDockTab>& EditorSettingsTab,
                                                         bool& bShouldCloseSettings, ISettingsModule& SettingModule)
{
	EditorSettingsTab = FGlobalTabmanager::Get()->FindExistingLiveTab(EditorSettingsName);
	bShouldCloseSettings = !EditorSettingsTab.IsValid();
	if (!EditorSettingsTab.IsValid() && !OpenEditorTab(EditorSettingsTab, SettingModule))
	{
		return nullptr;
	}
	const TSharedRef<SSettingsEditor> SettingsEditor = StaticCastSharedRef<SSettingsEditor>(
		EditorSettingsTab->GetContent());
	return StaticCastSharedPtr<SDetailsView>(*SettingsEditor.*GetPrivate(
		PrivateFieldHack::FGetter_SettingsView_From_SSettingsEditor()));
}

bool FPropertyHolder::OpenEditorTab(TSharedPtr<SDockTab>& EditorTab, ISettingsModule& SettingsModule)
{
	static const FName SettingsContainerName(TEXT("Editor"));

	TSharedPtr<ISettingsContainer> SettingsContainer = SettingsModule.GetContainer("Editor");
	TArray<TSharedPtr<ISettingsCategory>> Categories;
	SettingsContainer->GetCategories(Categories);
	if (SettingsContainer->GetCategories(Categories) == 0)
	{
		UE_LOG(LogPropertyHolder, Warning, TEXT("Cannot find any category to open settings tab and load properties"))
		return false;
	}
	TArray<TSharedPtr<ISettingsSection>> Sections;
	if (Categories[0]->GetSections(Sections) == 0)
	{
		UE_LOG(LogPropertyHolder, Warning, TEXT("Cannot find any section to open settings tab and load properties"))
		return false;
	}
	SettingsModule.ShowViewer(SettingsContainerName, Categories[0]->GetName(), Sections[0]->GetName());
	EditorTab = FGlobalTabmanager::Get()->FindExistingLiveTab(EditorSettingsName);
	if (!EditorTab.IsValid())
	{
		UE_LOG(LogPropertyHolder, Warning, TEXT("Cannot force to open Settings Tab"))
		return false;
	}
	return true;
}

template <bool bShouldLog>
TSharedPtr<FSectionDetail> FPropertyHolder::FindSectionByObjectAndCheck(
	TSharedRef<FDetailMultiTopLevelObjectRootNode> SectionNode,
	TMap<TSharedPtr<ISettingsCategory>, TSharedPtr<FCategoryDetail>>& FoundCategories,
	FSettingsDataCollection& OutResult, ISettingsModule& SettingModule, const ISettingsEditorModelPtr& Model,
	int DetailLayoutIndex)
{
	FDetailsObjectSet& RootObjectSet = (*SectionNode).*GetPrivate(
		PrivateFieldHack::FGetter_RootObjectSet_From_FDetailMultiTopLevelObjectRootNode());
	const UObject* RootObject = RootObjectSet.RootObjects[0];
	const TSharedPtr<ISettingsSection> Section = Model->GetSectionFromSectionObject(RootObject);

	if (!Section.IsValid())
	{
		UE_LOG(LogPropertyHolder, Warning, TEXT("Cannot find Settings Section for Root Object"))
		return nullptr;
	}
	if (const TSharedPtr<ISettingsCategory> Category = Section->GetCategory().Pin())
	{
		TSharedPtr<FCategoryDetail>& CategoryDetail = FoundCategories.FindOrAdd(Category);
		if (!CategoryDetail.IsValid())
		{
			CategoryDetail = MakeShared<FCategoryDetail>(SettingModule, Category);
			if constexpr (bShouldLog)
			{
				WriteLog(FString::Printf(TEXT("Category: '%s'\n"), *CategoryDetail->GetDisplayName().ToString()));
			}
			// FoundCategories.Add(Category) = CategoryDetail;
			TSharedRef<FCategoryDetail> CategoryDetailRef = CategoryDetail.ToSharedRef();
			OutResult.CategoryDetails.Add(CategoryDetailRef);
		}
		TSharedRef<FSectionDetail> SectionDetail = MakeShared<FSectionDetail>(
			CategoryDetail.ToSharedRef(), Section, SectionNode, DetailLayoutIndex);
		if constexpr (bShouldLog)
		{
			WriteLog(FString::Printf(TEXT("	Section: '%s'\n"), *(SectionDetail)->GetDisplayName().ToString()));
		}
		OutResult.SectionDetails.Add(SectionDetail);
		return SectionDetail;
	}
	UE_LOG(LogPropertyHolder, Warning, TEXT("Cannot find Settings Category for Root Object"))
	return nullptr;
}

template <bool bShouldLog>
void FPropertyHolder::FindAllInnerCategoryDetails(const TSharedRef<FDetailMultiTopLevelObjectRootNode>& SectionNode,
                                                  const TSharedRef<FSectionDetail>& SectionDetail,
                                                  FSettingsDataCollection& OutResult)
{
	TArray<TSharedRef<IDetailTreeNode>> InnerCategoryNodes;
	StaticCastSharedRef<FDetailTreeNode>(SectionNode)->GetChildren(InnerCategoryNodes);
	for (int InnerCategoryIndex = 0; InnerCategoryIndex < InnerCategoryNodes.Num(); ++ InnerCategoryIndex)
	{
		TSharedRef<IDetailTreeNode>& DetailTreeNode = InnerCategoryNodes[InnerCategoryIndex];
		if (DetailTreeNode->GetNodeType() != EDetailNodeType::Category)
		{
			continue;
		}
		TSharedRef<FDetailCategoryImpl> FDetailCategoryImplNode = StaticCastSharedRef<FDetailCategoryImpl>(
			DetailTreeNode);
		TSharedRef<FInnerCategoryDetail> InnerCategoryDetail = MakeShared<FInnerCategoryDetail>(
			SectionDetail, FDetailCategoryImplNode->GetDisplayName(), FDetailCategoryImplNode,
			InnerCategoryIndex);
		if constexpr (bShouldLog)
		{
			WriteLog(FString::Printf(
				TEXT("		InnerCategory: '%s'\n"), *(InnerCategoryDetail)->GetDisplayName().ToString()));
		}
		OutResult.InnerCategoryDetails.Add(InnerCategoryDetail);
		FindAllPropertyDetails<bShouldLog>(FDetailCategoryImplNode, InnerCategoryDetail, OutResult);
	}
}

template <bool bShouldLog>
void FPropertyHolder::FindAllPropertyDetails(TSharedRef<FDetailCategoryImpl>& CategoryNode,
                                             TSharedRef<FInnerCategoryDetail>& InnerCategoryDetail,
                                             FSettingsDataCollection& OutResult)
{
	bool bIsAdvancedSection = false;
	FDetailNodeList InnerCategoryChildren;
	CategoryNode->GetChildren(InnerCategoryChildren);
	for (int InnerCategoryChildIndex = 0; InnerCategoryChildIndex < InnerCategoryChildren.Num(); ++
	     InnerCategoryChildIndex)
	{
		TSharedRef<FDetailTreeNode>& InnerDetailTreeNode = InnerCategoryChildren[InnerCategoryChildIndex];
		if (InnerDetailTreeNode->GetNodeType() == EDetailNodeType::Advanced)
		{
			bIsAdvancedSection = true;
			continue;
		}
		FString DetailTreeNodeDisplayName = GetPropertyName(InnerDetailTreeNode);
		if constexpr (bShouldLog)
		{
			WriteLog(FString::Printf(TEXT("			Property: '%s'\n"), *DetailTreeNodeDisplayName));
		}
		OutResult.PropertyDetails.Add(MakeShared<FPropertyDetail>(
			InnerCategoryDetail, InnerDetailTreeNode, FText::FromString(DetailTreeNodeDisplayName),
			InnerCategoryChildIndex,
			bIsAdvancedSection));
	}
}

FString FPropertyHolder::GetPropertyName(TSharedRef<FDetailTreeNode> DetailNode)
{
	FString PropertyName;
	TArray<FString> OutFilterStrings;
	DetailNode->GetFilterStrings(OutFilterStrings);
	if (OutFilterStrings.Num() == 1 || OutFilterStrings.Num() == 2)
	{
		PropertyName = OutFilterStrings[0];
	}
	else if (OutFilterStrings.Num() == 0 && DetailNode->GetNodeType() == EDetailNodeType::Item)
	{
		const TSharedRef<FDetailItemNode> DetailItemNode = StaticCastSharedRef<FDetailItemNode>(
			DetailNode);
		const FDetailLayoutCustomization& Customization = (*DetailItemNode).*GetPrivate(
			PrivateFieldHack::FGetter_Customization_From_FDetailItemNode());
		if (Customization.DetailGroup)
		{
			PropertyName = (*Customization.DetailGroup.*GetPrivate(
					PrivateFieldHack::FGetter_LocalizedDisplayName_From_FDetailGroup())).
				ToString();
		}
	}
	else
	{
		UE_LOG(LogPropertyHolder, Log,
		       TEXT("Cannot get name for property node from settings. Filter strings array size = %d"),
		       OutFilterStrings.Num());
	}
	return PropertyName;
}

void FPropertyHolder::WriteLog(const FString& Text, bool bIsAppend)
{
	static FString FileLogPath = FPaths::ProjectPluginsDir() + "ExamplePlugin/Resources/PropertyLog.txt";
	FFileHelper::SaveStringToFile(Text, *FileLogPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(),
	                              (bIsAppend) ? FILEWRITE_Append : FILEWRITE_None);
}

const FName FPropertyHolder::EditorSettingsName = TEXT("EditorSettings");
