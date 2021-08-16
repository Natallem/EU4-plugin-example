#include "PropertyHolder.h"

#include "Details/CategoryDetail.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "Details/InnerCategoryDetail.h"
#include "Details/PropertyDetail.h"
#include "Details/SectionDetail.h"
#include "Multithreading/SearchTask.h"
#include "Misc/FileHelper.h"
#include "PropertyEditor/Private/SDetailsView.h"
#include "Details/AbstractSettingDetail.h"
#include "ISettingsEditorModel.h"
#include "ISettingsEditorModule.h"

FPropertyHolder::FPropertyHolder()
{
	ForceUpdateSettings();
	LoadProperties();
}

template <typename T>
T FPropertyHolder::AddToPropertyHolder(const T& Item)
{
	SettingDetails.Add(Item);
	SettingDetailsNames.Add(Item->GetDisplayName().ToString());
	return Item;
}

void FPropertyHolder::LoadProperties()
{
	struct FSectionSortPredicate
	{
		FORCEINLINE bool operator()(ISettingsSectionPtr A, ISettingsSectionPtr B) const
		{
			if (!A.IsValid() && !B.IsValid())
			{
				return false;
			}

			if (A.IsValid() != B.IsValid())
			{
				return B.IsValid();
			}

			return (A->GetDisplayName().CompareTo(B->GetDisplayName()) < 0);
		}
	};

	const TSharedPtr<ISettingsContainer> EditorSettingContainer = SettingsModule.GetContainer("Editor");
	TArray<ISettingsCategoryPtr> EditorSettingContainerCategories;
	EditorSettingContainer->GetCategories(EditorSettingContainerCategories);

	for (ISettingsCategoryPtr& Category : EditorSettingContainerCategories)
	{
		TArray<ISettingsSectionPtr> Sections;
		Category->GetSections(Sections);
		if (Sections.Num() == 0)
		{
			continue;
		}
		Sections.Sort(FSectionSortPredicate());
		TSharedRef<FCategoryDetail> CategoryDetail = AddToPropertyHolder(
			MakeShared<FCategoryDetail>(SettingsModule, Category, Sections[0]));
		for (const ISettingsSectionPtr& Section : Sections)
		{
			TSharedRef<FSectionDetail> SectionDetail = AddToPropertyHolder(
				MakeShared<FSectionDetail>(Section, CategoryDetail));

			UObject* SectionObject = Section->GetSettingsObject().Get();
			FString SectionObjectName = SectionObject->GetName();

			TMap<FString, TSharedRef<FInnerCategoryDetail>> PropertyCategoryMap;

			for (TFieldIterator<FProperty> PropertyIt(SectionObject->GetClass()); PropertyIt; ++PropertyIt)
			{
				const FString& PropertyCategoryName = PropertyIt->GetMetaData(TEXT("Category"));
				if (PropertyCategoryName.IsEmpty())
				{
					continue;
				}
				TSharedPtr<FInnerCategoryDetail> InnerCategoryDetail;
				if (!PropertyCategoryMap.Contains(PropertyCategoryName))
				{
					InnerCategoryDetail = AddToPropertyHolder<TSharedRef<FInnerCategoryDetail>>(
						MakeShared<FInnerCategoryDetail>(*PropertyIt, SectionDetail));
					InnerCategoryDetail = PropertyCategoryMap.Add(PropertyCategoryName,
					                                              InnerCategoryDetail.ToSharedRef());
				}
				else
				{
					InnerCategoryDetail = *PropertyCategoryMap.Find(PropertyCategoryName);
				}
				AddToPropertyHolder<TSharedRef<FPropertyDetail>>(
					MakeShared<FPropertyDetail>(SectionObject, *PropertyIt, InnerCategoryDetail.ToSharedRef()));
			}
		}
	}
}

TArray<int> FPropertyHolder::CreatePArray(const FString& Pattern)
{
	TArray<int> Result;
	Result.Init(-1, Pattern.Len());
	for (int r = 1, l = -1; r < Pattern.Len(); r++)
	{
		while (l != -1 && Pattern[l + 1] != Pattern[r])
			l = Result[l];
		if (Pattern[l + 1] == Pattern[r])
			Result[r] = ++l;
	}
	return Result;
}

TOptional<RequiredType> FPropertyHolder::FindNextWord(FSearchTask& Task, const FThreadSafeCounter& RequestCounter)
{
	static const int IterationBeforeCheck = 100; // Parameter
	int IterationCounter = 0;
	for (int i = Task.NextIndexToCheck; i < SettingDetailsNames.Num(); ++i)
	{
		++IterationCounter;
		if (IterationCounter == IterationBeforeCheck)
		{
			if (RequestCounter.GetValue() != Task.TaskId)
			{
				return TOptional<RequiredType>();
			}
			IterationCounter = 0;
		}
		if (IsSatisfiesRequest(SettingDetailsNames[i], Task.Request, Task.PArray))
		{
			Task.NextIndexToCheck = i + 1;
			return i;
		}
	}
	Task.NextIndexToCheck = SettingDetailsNames.Num();
	Task.bIsCompleteSearching = true;
	return TOptional<RequiredType>();
}

TSharedRef<const FAbstractSettingDetail> FPropertyHolder::GetSettingDetail(uint64 Index) const
{
	return SettingDetails[Index];
}

bool FPropertyHolder::IsSatisfiesRequest(const FString& StringInWhichWeFindPattern, const FString& Pattern,
                                         const TArray<int>& PArray)
{
	int tail = -1;

	for (int i = 0; i < StringInWhichWeFindPattern.Len(); i++)
	{
		while (tail != -1 && StringInWhichWeFindPattern[i] != Pattern[tail + 1])
			tail = PArray[tail];
		if (StringInWhichWeFindPattern[i] == Pattern[tail + 1])
			tail++;
		if (tail == Pattern.Len() - 1)
		{
			return true;
		}
	}
	return false;
}

void FPropertyHolder::LogAllProperties() const
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
	// DetailsView->SetOnDisplayedPropertiesChanged(
	// ::FOnDisplayedPropertiesChanged::CreateRaw(this, &SSearchEverywhereWidget::HandlePropertiesChanged));

	WriteLog("All properties log:\n", false);

	TSharedPtr<ISettingsContainer> EditorSettingContainer = SettingsModule.GetContainer("Editor");
	TArray<ISettingsCategoryPtr> EditorSettingContainerCategories;
	EditorSettingContainer->GetCategories(EditorSettingContainerCategories);
	for (const ISettingsCategoryPtr& Category : EditorSettingContainerCategories)
	{
		FName CategoryName = Category->GetName();
		const FText& CategoryDescription = Category->GetDescription();
		const FText& CategoryDisplayName = Category->GetDisplayName();
		WriteLog(FString::Printf(
			TEXT("CategoryDisplayName: '%s', CategoryName: '%s',CategoryDescription: '%s'\n"),

			*CategoryDisplayName.ToString(), *CategoryName.ToString(), *CategoryDescription.ToString()));

		TArray<ISettingsSectionPtr> Sections;
		Category->GetSections(Sections);
		Sections.Sort([](const ISettingsSectionPtr& A, const ISettingsSectionPtr& B)
		{
			return (A->GetDisplayName().CompareTo(B->GetDisplayName()) < 0);
		});
		for (const ISettingsSectionPtr& Section : Sections)
		{
			const FName& SectionName = Section->GetName();
			const FText& SectionDisplayName = Section->GetDisplayName();
			const FText& SectionDescription = Section->GetDescription();
			WriteLog(FString::Printf(
				TEXT("	SectionDisplayName: '%s',SectionName: '%s', SectionDescription: '%s'\n"),
				*SectionDisplayName.ToString(), *SectionName.ToString(), *SectionDescription.ToString()));
			FText PropertyDisplayName;
			TSet<FString> PropValues;
			TSet<FString> PathValues;
			TMap<FString, FProperty*> PropertyMap;

			UObject* SectionObject = Section->GetSettingsObject().Get();
			FString SectionObjectName = SectionObject->GetName();

			WriteLog(FString::Printf(TEXT("		SectionObjectName: '%s'\n"), *SectionObjectName));
			for (TFieldIterator<FProperty> PropertyIt(SectionObject->GetClass()); PropertyIt; ++PropertyIt)
			{
				const FString& PropertyCategoryName = PropertyIt->GetMetaData(TEXT("Category"));
				if (PropertyCategoryName.IsEmpty())
				{
					continue;
				}
				FString CppName = PropertyIt->GetNameCPP();
				PropValues.Add(CppName);
				PropertyMap.FindOrAdd(CppName) = *PropertyIt;
			}
			DetailsView->SetObject(SectionObject);
			TArray<FPropertyPath> paths = DetailsView->GetPropertiesInOrderDisplayed();
			for (FPropertyPath& path : paths)
			{
				PathValues.Add(path.ToString());
			}
			TSet<FString> Intersection = PropValues.Intersect(PathValues);
			TMap<FString, TArray<FProperty*>> PropertyCategoryMap;
			for (FString& PropertyName : Intersection)
			{
				const FString& PropertyCategoryName = PropertyMap[PropertyName]->GetMetaData(TEXT("Category"));
				FString DisplayPropertyCategoryName = FText::AsCultureInvariant(
					FName::NameToDisplayString(PropertyCategoryName, false)).ToString();

				PropertyCategoryMap.FindOrAdd(DisplayPropertyCategoryName).Add(PropertyMap[PropertyName]);
			}

			for (const TPair<FString, TArray<FProperty*>>& Pair : PropertyCategoryMap)
			{
				WriteLog(FString::Printf(TEXT("		PropertyCategoryName: '%s'\n"), *Pair.Key));
				for (FProperty* Property : Pair.Value)
				{
					WriteLog(FString::Printf(
						TEXT("			Both: '%s', Display name: '%s'\n"), *Property->GetNameCPP(),
						*Property->GetDisplayNameText().ToString()));
				}
			}

			TSet<FString> InPropNotInPath = PropValues.Difference(PathValues);
			TSet<FString> InPathNotInProp = PathValues.Difference(PropValues);
			if (InPathNotInProp.Num() != 0)
			{
				WriteLog(FString::Printf(
					TEXT("		Difference: in Paths, not in Properties(Category: '%s', Section: '%s'):\n"),
					*CategoryDisplayName.ToString(), *SectionDisplayName.ToString()));
				for (FString& Diff : InPathNotInProp)
				{
					WriteLog("			\'" + Diff + "\'\n");
				}
			}
			if (InPropNotInPath.Num() != 0)
			{
				WriteLog(FString::Printf(
					TEXT("		Difference: in Properties, not in Paths(Category: '%s', Section: '%s'):\n"),
					*CategoryDisplayName.ToString(), *SectionDisplayName.ToString()));
				for (FString& Diff : InPropNotInPath)
				{
					FString PropertyCategoryName = PropertyMap[Diff]->GetMetaData(TEXT("Category"));
					FString DisplayPropertyCategoryName = FText::AsCultureInvariant(
						FName::NameToDisplayString(PropertyCategoryName, false)).ToString();

					WriteLog(FString::Printf(
						TEXT("				'%s', Display name: '%s', PropertyCategory: '%s'\n"), *Diff,
						*PropertyMap[Diff]->GetDisplayNameText().ToString(), *DisplayPropertyCategoryName));
				}
			}
		}
	}
}

void FPropertyHolder::WriteLog(const FString& Text, bool IsAppend)
{
	static FString FileLogPath = FPaths::ProjectPluginsDir() + "ExamplePlugin/Resources/PropertyLog.txt";
	FFileHelper::SaveStringToFile(Text, *FileLogPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(),
	                              (IsAppend) ? FILEWRITE_Append : FILEWRITE_None);
}

FPropertyHolder& FPropertyHolder::Get()
{
	static FPropertyHolder Holder;
	return Holder;
}

/** Need to load AutoDiscoveredSettings in Setting Module. Otherwise not all properties will be discovered by data holder. */
void FPropertyHolder::ForceUpdateSettings() const
{
	const ISettingsContainerPtr SettingsContainer = SettingsModule.GetContainer("Editor");

	if (SettingsContainer.IsValid())
	{
		ISettingsEditorModule& SettingsEditorModule = FModuleManager::GetModuleChecked<ISettingsEditorModule>(
			"SettingsEditor");
		const ISettingsEditorModelRef SettingsEditorModel = SettingsEditorModule.CreateModel(
			SettingsContainer.ToSharedRef());

		SettingsEditorModule.CreateEditor(SettingsEditorModel);
	}
}
