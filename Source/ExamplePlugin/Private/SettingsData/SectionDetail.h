#pragma once
#include "ISettingsSection.h"
#include "ISettingDetail.h"

class FCategoryDetail;

class FSectionDetail : public ISettingDetail
{
public:
	FSectionDetail(const ISettingsSectionPtr& SettingsSection, const TSharedRef<FCategoryDetail>& CategoryDetail);

	virtual TSharedRef<SWidget> GetRowWidget() override;
	virtual FText GetDisplayName() override;

	TSharedRef<FCategoryDetail> CategoryDetail;
	ISettingsSectionPtr SettingsSection;
private:
	TSharedPtr<SWidget> RowWidget;
	void CreateRowWidget();
};
