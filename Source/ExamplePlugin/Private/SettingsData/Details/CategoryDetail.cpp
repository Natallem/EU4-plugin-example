#include "CategoryDetail.h"
#include "ISettingsModule.h"

FCategoryDetail::FCategoryDetail(ISettingsModule& SettingsModule,
                                 const ISettingsCategoryPtr& SettingCategory,
                                 const ISettingsSectionPtr& FirstSettingsSection): SettingsModule(SettingsModule),
	SettingCategory(SettingCategory),
	FirstSettingsSection(FirstSettingsSection)
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
