#pragma once

#include "ISettingsCategory.h"
#include "ISettingsSection.h"

#include "AbstractSettingDetail.h"

class ISettingsModule;

class FCategoryDetail : public FAbstractSettingDetail
{
public:
	FCategoryDetail(ISettingsModule& InSettingsModule, const ISettingsCategoryPtr& InSettingCategory);

	virtual FText GetDisplayName() const override;
	virtual FName GetName() const override;
	virtual void DoAction() override;
	virtual FString GetPath() const override;

	ISettingsModule& SettingsModule;
	ISettingsCategoryPtr SettingCategory;
private:
	FText CategoryDisplayName;
	ISettingsSectionPtr FirstSettingsSection;

	/** @return First section that displays in Settings Editor Tab */
	TSharedPtr<ISettingsSection> GetFirstSection() const;
};
