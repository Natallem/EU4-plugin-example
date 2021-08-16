#pragma once

#include "CoreMinimal.h"

#include "AbstractSettingDetail.h"

class SWidget;
class FProperty;
class FSectionDetail;

class FInnerCategoryDetail : public FAbstractSettingDetail
{
public:
	FInnerCategoryDetail(FProperty* InProperty, const TSharedRef<FSectionDetail>& InSectionDetail);

	virtual FText GetDisplayName() const override;
	virtual FName GetName() const override;
	virtual void DoAction() const override;
	virtual FString GetPath() const override;

	TSharedRef<FSectionDetail> SectionDetail;
private:
	FText CategoryDisplayName;
};
