#pragma once

#include "ISettingsSection.h"

#include "AbstractSettingDetail.h"

class FCategoryDetail;
class FDetailMultiTopLevelObjectRootNode;
class FSectionDetail : public FAbstractSettingDetail
{
public:

	FSectionDetail(const TSharedRef<FCategoryDetail>& CategoryDetail, const ISettingsSectionPtr& SettingsSection,
	               const TWeakPtr<FDetailMultiTopLevelObjectRootNode>& SettingTreeNode, int SettingIndex);

	virtual FText GetDisplayName() const override;

	virtual FName GetName() const override;

	virtual void DoAction() override;

	virtual FString GetPath() const override;
	
	TSharedRef<FCategoryDetail> CategoryDetail;
	ISettingsSectionPtr SettingsSection;
	TWeakPtr<FDetailMultiTopLevelObjectRootNode> SettingTreeNode;
	int SettingIndex;
	FText SectionDisplayName;
};
