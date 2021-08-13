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

TSharedRef<SWidget> FPropertyDetail::GetRowWidget()
{
	if (!RowWidget.IsValid())
	{
		CreateRowWidget();
	}
	return RowWidget.ToSharedRef();
}

FText FPropertyDetail::GetDisplayName()
{
	return Property->GetDisplayNameText();
}

void FPropertyDetail::CreateRowWidget()
{
	SAssignNew(RowWidget, SButton)
		.Text(GetDisplayName())
		.OnClicked_Lambda([this]()
	                              {
		                              InnerCategoryDetail->SectionDetail->CategoryDetail->SettingsModule.ShowViewer(
			                              FName("Editor"),
			                              FName(InnerCategoryDetail->SectionDetail->CategoryDetail->GetDisplayName().
			                                                         ToString()),
			                              FName(InnerCategoryDetail->SectionDetail->GetDisplayName().ToString()));
		                              return FReply::Handled();
	                              });
}
