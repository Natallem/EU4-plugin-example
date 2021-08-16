#pragma once

#include "AbstractSettingDetail.h"
#include "ISettingsSection.h"

class ISettingsModule;
class FInnerCategoryDetail;

class FPropertyDetail : public FAbstractSettingDetail
{
public:
	FPropertyDetail(UObject* SettingObject, FProperty* Property,
	                const TSharedRef<FInnerCategoryDetail>& InnerCategoryDetail);

	virtual FText GetDisplayName() const override;
	virtual FName GetName() const override;
	virtual void DoAction() const override;

private:
	UObject* SettingObject;
	FProperty* Property;
	TSharedRef<FInnerCategoryDetail> InnerCategoryDetail;
};
