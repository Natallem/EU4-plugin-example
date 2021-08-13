#pragma once

#include "ISettingDetail.h"
#include "ISettingsSection.h"

class ISettingsModule;
class FInnerCategoryDetail;

class FPropertyDetail : public ISettingDetail
{
public:
	FPropertyDetail(UObject* SettingObject, FProperty* Property,
		const TSharedRef<FInnerCategoryDetail>& InnerCategoryDetail);

	virtual TSharedRef<SWidget> GetRowWidget() override;
	virtual FText GetDisplayName() override;

private:
	UObject* SettingObject;
	FProperty* Property;
	TSharedRef<FInnerCategoryDetail> InnerCategoryDetail;
	
	TSharedPtr<SWidget> RowWidget;
	void CreateRowWidget();
};
