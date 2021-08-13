#include "CategoryDetail.h"

#include "ISettingsModule.h"

FCategoryDetail::FCategoryDetail(ISettingsModule& SettingsModule,
	const ISettingsCategoryPtr& SettingCategory, const ISettingsSectionPtr& FirstSettingsSection): SettingsModule(SettingsModule),
	SettingCategory(SettingCategory),
	FirstSettingsSection(FirstSettingsSection)
{
}

TSharedRef<SWidget> FCategoryDetail::GetRowWidget()
{
	if (!RowWidget.IsValid())
	{
		CreateRowWidget();
	}
	return RowWidget.ToSharedRef();
}

FText FCategoryDetail::GetDisplayName()
{
	return SettingCategory->GetDisplayName();
}

void FCategoryDetail::CreateRowWidget()
{
	SAssignNew(RowWidget, SButton)
		.Text(GetDisplayName())
		.OnClicked_Lambda([this]()
	                              {
		                              SettingsModule.ShowViewer(FName("Editor"), FName(GetDisplayName().ToString()),
		                                                        FName(FirstSettingsSection->GetDisplayName().ToString()));
		                              return FReply::Handled();
	                              });
}
