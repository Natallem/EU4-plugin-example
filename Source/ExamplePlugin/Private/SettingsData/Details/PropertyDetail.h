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

	TWeakPtr<FDetailTreeNode> PropertyDetailTreeNode;


	TSharedRef<FInnerCategoryDetail> InnerCategoryDetail;
private:
	FText PropertyDisplayName;

	/** Planed to be used in FPropertyHolder::UpdateTreeNodes for fast searching Tree Node in case PropertyDetailTreeNode is inaccessible. Not implemented*/
	int SettingIndex;

	/** Show wither this property is advanced. Unused, may be deleted*/
	bool bIsAdvanced;

	/* Used to get the notification from timer to unhighlight property */
	FTimerHandle TimerHandle;
};
