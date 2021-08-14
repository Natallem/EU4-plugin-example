#include "InnerCategoryDetail.h"

#include "CategoryDetail.h"
#include "ISettingsModule.h"
#include "SectionDetail.h"
#include "Widgets/Input/SButton.h"

FInnerCategoryDetail::FInnerCategoryDetail(FProperty* Property,
                                           const TSharedRef<FSectionDetail>& SectionDetail):
	SectionDetail(SectionDetail)
{
	CategoryDisplayName = FText::AsCultureInvariant(
		FName::NameToDisplayString(Property->GetMetaData(TEXT("Category")), false));
}

FText FInnerCategoryDetail::GetDisplayName() const
{
	return CategoryDisplayName;
}

void FInnerCategoryDetail::DoAction() const
{
	SectionDetail->CategoryDetail->SettingsModule.ShowViewer(
										FName("Editor"), FName(  SectionDetail->CategoryDetail->GetDisplayName().ToString()),
										FName(SectionDetail->GetDisplayName().ToString()));
}
