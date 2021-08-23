#pragma once

#include "AbstractSettingDetail.h"

class ISettingsModule;
class FInnerCategoryDetail;
class FDetailTreeNode;

class FPropertyDetail : public FAbstractSettingDetail
{
public:
	FPropertyDetail(const TSharedRef<FInnerCategoryDetail>& InnerCategoryDetail,
		const TWeakPtr<FDetailTreeNode>& SettingDetail, const FText& PropertyDisplayName, int SettingIndex,
		bool bIsAdvanced);

	virtual FText GetDisplayName() const override;
	virtual void DoAction() override;
	virtual FString GetPath() const override;

private:
	TSharedRef<FInnerCategoryDetail> InnerCategoryDetail;
	TWeakPtr<FDetailTreeNode> SettingDetail;
	FText PropertyDisplayName;
	int SettingIndex;
	bool bIsAdvanced;
};
