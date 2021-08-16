#pragma once

#include "AbstractSettingDetail.h"

class ISettingsModule;
class FInnerCategoryDetail;

class FPropertyDetail : public FAbstractSettingDetail
{
public:
	FPropertyDetail(UObject* InSettingObject, FProperty* InProperty,
	                const TSharedRef<FInnerCategoryDetail>& InnerCategoryDetail);

	virtual FText GetDisplayName() const override;
	virtual FName GetName() const override;
	virtual void DoAction() const override;
	virtual FString GetPath() const override;

private:
	UObject* SettingObject;
	FProperty* Property;
	TSharedRef<FInnerCategoryDetail> InnerCategoryDetail;
};
