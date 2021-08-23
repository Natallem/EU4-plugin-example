#include "PropertyDetail.h"

#include "ISettingsModule.h"

#include "CategoryDetail.h"
#include "InnerCategoryDetail.h"
#include "SectionDetail.h"
#include "PropertyEditor/Private/SDetailsViewBase.h"


FPropertyDetail::FPropertyDetail(const TSharedRef<FInnerCategoryDetail>& InnerCategoryDetail,
	const TWeakPtr<FDetailTreeNode>& SettingDetail, const FText& PropertyDisplayName, int SettingIndex,
	bool bIsAdvanced): InnerCategoryDetail(InnerCategoryDetail),
	                   SettingDetail(SettingDetail),
	                   PropertyDisplayName(PropertyDisplayName),
	                   SettingIndex(SettingIndex),
	                   bIsAdvanced(bIsAdvanced)
{
}

FText FPropertyDetail::GetDisplayName() const
{
	return PropertyDisplayName;
}

void FPropertyDetail::DoAction() 
{
	InnerCategoryDetail->SectionDetail->CategoryDetail->SettingsModule.ShowViewer(
		FName("Editor"),
		InnerCategoryDetail->SectionDetail->CategoryDetail->GetName(),
		InnerCategoryDetail->SectionDetail->GetName());
	SetTextInSearchBox(GetSDetailsView(), GetDisplayName());
}

FString FPropertyDetail::GetPath() const
{
	return InnerCategoryDetail->GetPath() + Delimiter + GetDisplayName().ToString();
}
