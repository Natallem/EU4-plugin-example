#pragma once

#include "ISettingsCategory.h"
#include "ISettingsSection.h"

#include "AbstractSettingDetail.h"

class ISettingsModule;

class FCategoryDetail : public FAbstractSettingDetail
{
public:
	FCategoryDetail(ISettingsModule& InSettingsModule, const ISettingsCategoryPtr& InSettingCategory,
	                const ISettingsSectionPtr& InFirstSettingsSection);

	virtual FText GetDisplayName() const override;
	virtual FName GetName() const override;
	virtual void DoAction() const override;
	virtual FString GetPath() const override;

	ISettingsModule& SettingsModule;
	ISettingsCategoryPtr SettingCategory;
private:
	ISettingsSectionPtr FirstSettingsSection;
};
