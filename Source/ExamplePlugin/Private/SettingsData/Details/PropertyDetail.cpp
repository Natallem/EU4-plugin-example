#include "PropertyDetail.h"

#include "ISettingsModule.h"

#include "CategoryDetail.h"
#include "InnerCategoryDetail.h"
#include "SectionDetail.h"

FPropertyDetail::FPropertyDetail(UObject* InSettingObject, FProperty* InProperty,
                                 const TSharedRef<FInnerCategoryDetail>& InnerCategoryDetail):
	SettingObject(InSettingObject),
	Property(InProperty),
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
	Foo();
}

FString FPropertyDetail::GetPath() const
{
	return InnerCategoryDetail->GetPath() + Delimiter + GetDisplayName().ToString();
}
