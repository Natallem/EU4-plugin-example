#pragma once

#include "CoreMinimal.h"
#include "AbstractSettingDetail.h"

class SWidget;
class FProperty;
class FSectionDetail;

class FInnerCategoryDetail : public FAbstractSettingDetail
{
public:
	FInnerCategoryDetail(FProperty* Property, const TSharedRef<FSectionDetail>& SectionDetail);

	virtual FText GetDisplayName() const override;
	virtual void DoAction() const override;

	TSharedRef<FSectionDetail> SectionDetail;
private:
	FText CategoryDisplayName;
};
