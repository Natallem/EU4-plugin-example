#include "SectionDetail.h"

#include "ISettingsModule.h"
#include "CategoryDetail.h"

FSectionDetail::FSectionDetail(const ISettingsSectionPtr& SettingsSection,
                               const TSharedRef<FCategoryDetail>& CategoryDetail)
	: SettingsSection(SettingsSection),
	  CategoryDetail(CategoryDetail)
{
}

TSharedRef<SWidget> FSectionDetail::GetRowWidget()
{
	if (!RowWidget.IsValid())
	{
		CreateRowWidget();
	}
	return RowWidget.ToSharedRef();
}

FText FSectionDetail::GetDisplayName()
{
	return SettingsSection->GetDisplayName();
}

void FSectionDetail::CreateRowWidget()
{
	SAssignNew(RowWidget, SButton)
		.Text(GetDisplayName())
		.OnClicked_Lambda([this]() -> FReply
	                              {
		                              CategoryDetail
			                              ->SettingsModule.ShowViewer(FName("Editor"),
			                                                          FName(CategoryDetail->GetDisplayName().
				                                                          ToString()),
			                                                          FName(GetDisplayName().ToString()));
		                              return FReply::Handled();
	                              });
}
