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

	/** WeakPtr for TreeNode in SDetailsView of last opened Setting Window. Used to expand this node in order to show highlighted node. @see FPropertyDetail::DoAction() */
	TWeakPtr<FDetailCategoryImpl> CategoryTreeNode;
private:
	FText InnerCategoryDisplayName;

	/** Planed to be used in FPropertyHolder::UpdateTreeNodes for fast searching Tree Node in case CategoryTreeNode is inaccessible. Not implemented*/
	int SettingsIndex;
};
