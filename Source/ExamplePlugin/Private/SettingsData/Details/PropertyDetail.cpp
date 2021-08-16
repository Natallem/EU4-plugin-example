#include "PropertyDetail.h"

#include "CategoryDetail.h"
#include "InnerCategoryDetail.h"
#include "ISettingsModule.h"
#include "SectionDetail.h"
#include "Widgets/Input/SButton.h"

FPropertyDetail::FPropertyDetail(UObject* SettingObject, FProperty* Property,
                                 const TSharedRef<FInnerCategoryDetail>& InnerCategoryDetail):
	SettingObject(SettingObject),
	Property(Property),
	InnerCategoryDetail(InnerCategoryDetail)
{
}

FText FPropertyDetail::GetDisplayName() const
{
	return Property->GetDisplayNameText();
}

FName FPropertyDetail::GetName() const
{
	return FName();
}

void FPropertyDetail::DoAction() const
{
	InnerCategoryDetail->SectionDetail->CategoryDetail->SettingsModule.ShowViewer(
		FName("Editor"),
		InnerCategoryDetail->SectionDetail->CategoryDetail->GetName(),
		InnerCategoryDetail->SectionDetail->GetName());
}
