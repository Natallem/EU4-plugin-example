#pragma once
#include "ISettingDetail.h"
#include "CoreMinimal.h"

class SWidget;
class FProperty;
class FSectionDetail;

class FInnerCategoryDetail : public ISettingDetail
{
public:
	FInnerCategoryDetail(FProperty* Property, const TSharedRef<FSectionDetail>& SectionDetail);

	virtual TSharedRef<SWidget> GetRowWidget() override;
	virtual FText GetDisplayName() override;

	TSharedRef<FSectionDetail> SectionDetail;
private:
	FText CategoryDisplayName;
	TSharedPtr<SWidget> RowWidget;

	void CreateRowWidget();
};
