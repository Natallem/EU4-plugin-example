#include "InnerCategoryDetail.h"

#include "ISettingsModule.h"
#include "Widgets/Input/SButton.h"

#include "CategoryDetail.h"
#include "SectionDetail.h"

FInnerCategoryDetail::FInnerCategoryDetail(FProperty* InProperty, const TSharedRef<FSectionDetail>& InSectionDetail)
	: SectionDetail(InSectionDetail)
{
	CategoryDisplayName = FText::AsCultureInvariant(
		FName::NameToDisplayString(InProperty->GetMetaData(TEXT("Category")), false));
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

FString FInnerCategoryDetail::GetPath() const
{
	return SectionDetail->GetPath() + Delimiter + GetDisplayName().ToString();
}
