#pragma once

#include "ISettingsCategory.h"
#include "ISettingsSection.h"

#include "AbstractSettingDetail.h"

class ISettingsModule;

class FCategoryDetail : public FAbstractSettingDetail
{
public:
	FCategoryDetail(ISettingsModule& SettingsModule, const ISettingsCategoryPtr& SettingCategory,
	                const ISettingsSectionPtr& FirstSettingsSection);

	virtual FText GetDisplayName() const override;
	virtual FName GetName() const override;
	virtual void DoAction() const override;

	ISettingsModule& SettingsModule;
	ISettingsCategoryPtr SettingCategory;
private:
	ISettingsSectionPtr FirstSettingsSection;
};
