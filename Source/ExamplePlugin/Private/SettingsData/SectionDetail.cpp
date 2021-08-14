#include "SectionDetail.h"

#include "ISettingsModule.h"
#include "CategoryDetail.h"
#include "Widgets/Text/STextBlock.h"

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

void FSectionDetail::DoAction() const
{
	CategoryDetail->SettingsModule.ShowViewer(FName("Editor"),
	                                          FName(CategoryDetail->GetDisplayName().
	                                                                ToString()),
	                                          FName(GetDisplayName().ToString()));
}
