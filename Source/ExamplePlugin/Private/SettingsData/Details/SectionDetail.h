#pragma once
#include "AbstractSettingDetail.h"
#include "ISettingsSection.h"

class FCategoryDetail;

class FSectionDetail : public FAbstractSettingDetail
{
public:
	FSectionDetail(const ISettingsSectionPtr& SettingsSection, const TSharedRef<FCategoryDetail>& CategoryDetail);

	virtual FText GetDisplayName() const override;
	virtual FName GetName() const override;
	virtual void DoAction() const override;

	TSharedRef<FCategoryDetail> CategoryDetail;
	ISettingsSectionPtr SettingsSection;
};
