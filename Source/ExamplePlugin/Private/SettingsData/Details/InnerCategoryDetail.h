#pragma once

#include "CoreMinimal.h"

#include "AbstractSettingDetail.h"

class SWidget;
class FProperty;
class FSectionDetail;
class FDetailCategoryImpl;

class FInnerCategoryDetail : public FAbstractSettingDetail
{
public:
	FInnerCategoryDetail(const TSharedRef<FSectionDetail>& SectionDetail, const FText& InnerCategoryDisplayName,
		const TWeakPtr<FDetailCategoryImpl>& CategoryTreeNode, int SettingsIndex);

	virtual FText GetDisplayName() const override;
	virtual void DoAction() override;
	virtual FString GetPath() const override;

	TSharedRef<FSectionDetail> SectionDetail;
	TWeakPtr<FDetailCategoryImpl> CategoryTreeNode;
private:
	FText InnerCategoryDisplayName;
	int SettingsIndex;
};
