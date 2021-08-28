#include "CategoryDetail.h"
#include "ISettingsModule.h"

FCategoryDetail::FCategoryDetail(ISettingsModule& InSettingsModule,
                                 const ISettingsCategoryPtr& InSettingCategory)
	: SettingsModule(InSettingsModule),
	  SettingCategory(InSettingCategory)
{
	FirstSettingsSection = GetFirstSection();
	CategoryDisplayName = SettingCategory->GetDisplayName();
}

FText FCategoryDetail::GetDisplayName() const
{
	return CategoryDisplayName;
}

FName FCategoryDetail::GetName() const
{
	return SettingCategory->GetName();
}

void FCategoryDetail::DoAction()
{
	if (!FirstSettingsSection.IsValid())
	{
		FirstSettingsSection = GetFirstSection();
		if (!FirstSettingsSection.IsValid())
		{
			UE_LOG(LogSearchSettingDetail, Warning,
			       TEXT("Cannot open Settings on first Section because no sections detected"))
			SettingsModule.ShowViewer(FName("Editor"),
			                          GetName(),
			                          FName());
			return;
		}
	}
	SettingsModule.ShowViewer(FName("Editor"),
	                          GetName(),
	                          FirstSettingsSection->GetName());
}

FString FCategoryDetail::GetPath() const
{
	return GetDisplayName().ToString();
}

TSharedPtr<ISettingsSection> FCategoryDetail::GetFirstSection() const
{
	TArray<TSharedPtr<ISettingsSection>> Sections;
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

	if (SettingCategory->GetSections(Sections) != 0)
	{
		Sections.Sort(FSectionSortPredicate());
		return Sections[0];
	}
	return nullptr;
}
