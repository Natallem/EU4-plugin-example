#include "PropertyHolder.h"

#include "Details/CategoryDetail.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "Misc/FileHelper.h"
#include "IDetailTreeNode.h"
#include "PropertyPath.h"
#include "Details/AbstractSettingDetail.h"
#include "Details/InnerCategoryDetail.h"
#include "Details/PropertyDetail.h"
#include "Details/SectionDetail.h"
#include "Multithreading/SearchTask.h"
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

DEFINE_LOG_CATEGORY(LogPropertyHolder);

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
	return CategoryDetails.Num() + SectionDetails.Num() + InnerCategoryDetails.Num() + PropertyDetails.Num();
}

TSharedRef<FAbstractSettingDetail> FSettingsDataCollection::operator[](int Index) const
{
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

TArray<int> FPropertyHolder::CreatePArray(const FString& InPattern)
{
	TArray<int> Result;
	Result.Init(-1, InPattern.Len());
	for (int r = 1, l = -1; r < InPattern.Len(); r++)
	{
		while (l != -1 && InPattern[l + 1] != InPattern[r])
			l = Result[l];
		if (InPattern[l + 1] == InPattern[r])
			Result[r] = ++l;
	}
	return Result;
}

TOptional<RequiredType> FPropertyHolder::FindNextWord(FSearchTask& OutTask, const FThreadSafeCounter& InRequestCounter)
{
	static const int IterationBeforeCheck = 100; // Parameter
	int IterationCounter = 0;
	for (int i = OutTask.NextIndexToCheck; i < Data.Num(); ++i)
	{
		++IterationCounter;
		if (IterationCounter == IterationBeforeCheck)
		{
			if (InRequestCounter.GetValue() != OutTask.TaskId)
			{
				return TOptional<RequiredType>();
			}
			IterationCounter = 0;
		}
		if (IsSatisfiedRequest(Data[i]->GetDisplayName().ToString(), OutTask.RequestString, OutTask.PArray))
		{
			OutTask.NextIndexToCheck = i + 1;
			return i;
		}
	}
	OutTask.NextIndexToCheck = Data.Num();
	OutTask.bIsCompleteSearching = true;
	return TOptional<RequiredType>();
}

TSharedRef<FAbstractSettingDetail> FPropertyHolder::GetSettingDetail(uint64 Index) const
{
	return Data[Index];
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
	TMap<TSharedPtr<ISettingsCategory>, TSharedPtr<FCategoryDetail>> FoundCategories;

	TSharedPtr<SDockTab> EditorSettingsTab = FGlobalTabmanager::Get()->FindExistingLiveTab(EditorSettingsName);
	const bool bShouldCloseSettings = !EditorSettingsTab.IsValid();
	if (!EditorSettingsTab.IsValid() && !OpenEditorTab(EditorSettingsTab, SettingModule))
	{
		return Result;
	}

	const TSharedRef<SSettingsEditor> SettingsEditor = StaticCastSharedRef<SSettingsEditor>(
		EditorSettingsTab->GetContent());
	const TSharedPtr<SDetailsView> DetailsView = StaticCastSharedPtr<SDetailsView>(*SettingsEditor.*GetPrivate(
		PrivateFieldHack::FGetter_SettingsView_From_SSettingsEditor()));

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

bool FPropertyHolder::IsSatisfiedRequest(const FString& InStringToFindPattern, const FString& InPattern,
                                         const TArray<int>& PArray)
{
	int Tail = -1;

	for (int i = 0; i < InStringToFindPattern.Len(); i++)
	{
		while (Tail != -1 && InStringToFindPattern[i] != InPattern[Tail + 1])
			Tail = PArray[Tail];
		if (InStringToFindPattern[i] == InPattern[Tail + 1])
			Tail++;
		if (Tail == InPattern.Len() - 1)
		{
			return true;
		}
	}
	return false;
}

void FPropertyHolder::WriteLog(const FString& Text, bool bIsAppend)
{
	static FString FileLogPath = FPaths::ProjectPluginsDir() + "ExamplePlugin/Resources/PropertyLog_temp.txt";
	FFileHelper::SaveStringToFile(Text, *FileLogPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(),
	                              (bIsAppend) ? FILEWRITE_Append : FILEWRITE_None);
}

const FName FPropertyHolder::EditorSettingsName = TEXT("EditorSettings"); 
