#include "InnerCategoryDetail.h"

#include "CategoryDetail.h"
#include "ISettingsModule.h"
#include "SectionDetail.h"

FInnerCategoryDetail::FInnerCategoryDetail(FProperty* Property,
                                           const TSharedRef<FSectionDetail>& SectionDetail):
	SectionDetail(SectionDetail)
{
	CategoryDisplayName = FText::AsCultureInvariant(
		FName::NameToDisplayString(Property->GetMetaData(TEXT("Category")), false));
}

TSharedRef<SWidget> FInnerCategoryDetail::GetRowWidget()
{
	if (!RowWidget.IsValid())
	{
		CreateRowWidget();
	}
	return RowWidget.ToSharedRef();
}

FText FInnerCategoryDetail::GetDisplayName()
{
	return CategoryDisplayName;
}

void FInnerCategoryDetail::CreateRowWidget()
{
	SAssignNew(RowWidget, SButton)
		.Text(GetDisplayName())
		.OnClicked_Lambda([this]()
	                              {
		                              SectionDetail->CategoryDetail->SettingsModule.ShowViewer(
			                              FName("Editor"), FName(  SectionDetail->CategoryDetail->GetDisplayName().ToString()),
			                              FName(SectionDetail->GetDisplayName().ToString()));
		                              return FReply::Handled();
	                              });
}
