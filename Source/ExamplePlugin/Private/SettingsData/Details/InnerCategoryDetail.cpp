#include "InnerCategoryDetail.h"

#include "ISettingsModule.h"
#include "Widgets/Input/SButton.h"

#include "CategoryDetail.h"
#include "SectionDetail.h"

FInnerCategoryDetail::FInnerCategoryDetail(FProperty* Property, const TSharedRef<FSectionDetail>& SectionDetail)
	: SectionDetail(SectionDetail)
{
	CategoryDisplayName = FText::AsCultureInvariant(
		FName::NameToDisplayString(Property->GetMetaData(TEXT("Category")), false));
}

FText FInnerCategoryDetail::GetDisplayName() const
{
	return CategoryDisplayName;
}

FName FInnerCategoryDetail::GetName() const
{
	return FName();
}

void FInnerCategoryDetail::DoAction() const
{
	SectionDetail->CategoryDetail->SettingsModule.ShowViewer(
		FName("Editor"),
		SectionDetail->CategoryDetail->GetName(),
		SectionDetail->GetName());
}
