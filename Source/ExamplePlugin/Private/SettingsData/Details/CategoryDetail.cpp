#include "CategoryDetail.h"
#include "ISettingsModule.h"

FCategoryDetail::FCategoryDetail(ISettingsModule& InSettingsModule,
                                 const ISettingsCategoryPtr& InSettingCategory,
                                 const ISettingsSectionPtr& InFirstSettingsSection)
	: SettingsModule(InSettingsModule),
	  SettingCategory(InSettingCategory),
	  FirstSettingsSection(InFirstSettingsSection)
{
}

FText FCategoryDetail::GetDisplayName() const
{
	return SettingCategory->GetDisplayName();
}

FName FCategoryDetail::GetName() const
{
	return SettingCategory->GetName();
}

void FCategoryDetail::DoAction() const
{
	SettingsModule.ShowViewer(FName("Editor"),
	                          GetName(),
	                          FirstSettingsSection->GetName());
}

FString FCategoryDetail::GetPath() const
{
	return GetDisplayName().ToString();
}
