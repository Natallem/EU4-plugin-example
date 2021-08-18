#include "SectionDetail.h"

#include "CategoryDetail.h"
#include "ISettingsModule.h"
#include "PropertyEditor/Private/SDetailsView.h"
#include "PropertyPath.h"

FSectionDetail::FSectionDetail(const ISettingsSectionPtr& InSettingsSection,
                               const TSharedRef<FCategoryDetail>& InCategoryDetail)
	: CategoryDetail(InCategoryDetail),
	  SettingsSection(InSettingsSection)
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
	TSharedPtr<SDetailsView> DetailsView = GetSDetailsView();
	 TArray<FPropertyPath> PropertyPaths = DetailsView->GetPropertiesInOrderDisplayed();
}

FString FSectionDetail::GetPath() const
{
	return CategoryDetail->GetPath() + Delimiter + GetDisplayName().ToString();
}
