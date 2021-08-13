#pragma once

#include "ISettingDetail.h"
#include "ISettingsCategory.h"
#include "ISettingsSection.h"

class ISettingsModule;

class FCategoryDetail : public ISettingDetail
{
public:
	FCategoryDetail(ISettingsModule& SettingsModule, const ISettingsCategoryPtr& SettingCategory,
	                const ISettingsSectionPtr& FirstSettingsSection);

	virtual TSharedRef<SWidget> GetRowWidget() override;
	virtual FText GetDisplayName() override;


	ISettingsModule& SettingsModule;
	ISettingsCategoryPtr SettingCategory;
private:
	TSharedPtr<SWidget> RowWidget;
	ISettingsSectionPtr FirstSettingsSection;
	
	void CreateRowWidget();
};
