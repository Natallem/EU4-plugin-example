#pragma once

#include "AbstractSettingDetail.h"
#include "Engine/EngineTypes.h"

class ISettingsModule;
class FInnerCategoryDetail;
class FDetailTreeNode;

class FPropertyDetail : public FAbstractSettingDetail, public TSharedFromThis<FPropertyDetail>
{
public:
	FPropertyDetail(const TSharedRef<FInnerCategoryDetail>& InnerCategoryDetail,
		const TWeakPtr<FDetailTreeNode>& SettingDetail, const FText& PropertyDisplayName, int SettingIndex,
		bool bIsAdvanced);

	virtual FText GetDisplayName() const override;
	virtual void DoAction() override;
	virtual FString GetPath() const override;

	TWeakPtr<FDetailTreeNode> SettingDetail;
	FTimerHandle TimerHandle;


	TSharedRef<FInnerCategoryDetail> InnerCategoryDetail;
private:
	FText PropertyDisplayName;
	int SettingIndex;
	bool bIsAdvanced;
};
