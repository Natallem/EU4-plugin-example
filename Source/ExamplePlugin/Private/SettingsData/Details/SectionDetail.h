#pragma once

#include "ISettingsSection.h"

#include "AbstractSettingDetail.h"

class FCategoryDetail;

class FSectionDetail : public FAbstractSettingDetail
{
public:
	FSectionDetail(const ISettingsSectionPtr& InSettingsSection, const TSharedRef<FCategoryDetail>& InCategoryDetail);

	virtual FText GetDisplayName() const override;
	virtual FName GetName() const override;
	virtual void DoAction() const override;
	virtual FString GetPath() const override;

	TSharedRef<FCategoryDetail> CategoryDetail;
	ISettingsSectionPtr SettingsSection;
};
