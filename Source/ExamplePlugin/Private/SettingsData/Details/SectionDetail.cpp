#include "SectionDetail.h"

#include "CategoryDetail.h"
#include "ISettingsModule.h"

FSectionDetail::FSectionDetail(const ISettingsSectionPtr& SettingsSection,
                               const TSharedRef<FCategoryDetail>& CategoryDetail)
	: CategoryDetail(CategoryDetail),
	  SettingsSection(SettingsSection)
{
}

FText FSectionDetail::GetDisplayName() const
{
	return SettingsSection->GetDisplayName();
}

FName FSectionDetail::GetName() const
{
	return SettingsSection->GetName();
}

void FSectionDetail::DoAction() const
{
	CategoryDetail->SettingsModule.ShowViewer(FName("Editor"),
	                                          CategoryDetail->GetName(),
	                                          GetName());
}
