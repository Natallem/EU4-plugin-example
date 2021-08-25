#include "PropertyDetail.h"

#include "ISettingsModule.h"

#include "CategoryDetail.h"
#include "Editor.h"
#include "InnerCategoryDetail.h"
#include "SectionDetail.h"
#include "PropertyEditor/Private/DetailCategoryBuilderImpl.h"
#include "PropertyEditor/Private/SDetailsViewBase.h"
#include "SettingsData/PropertyHolder.h"


FPropertyDetail::FPropertyDetail(const TSharedRef<FInnerCategoryDetail>& InnerCategoryDetail,
                                 const TWeakPtr<FDetailTreeNode>& SettingDetail, const FText& PropertyDisplayName,
                                 int SettingIndex,
                                 bool bIsAdvanced)
	: SettingDetail(SettingDetail),
	  InnerCategoryDetail(InnerCategoryDetail),
	  PropertyDisplayName(PropertyDisplayName),
	  SettingIndex(SettingIndex),
	  bIsAdvanced(bIsAdvanced)
{
}

FText FPropertyDetail::GetDisplayName() const
{
	return PropertyDisplayName;
}

void FPropertyDetail::DoAction()
{
	InnerCategoryDetail->SectionDetail->CategoryDetail->SettingsModule.ShowViewer(
		FName("Editor"),
		InnerCategoryDetail->SectionDetail->CategoryDetail->GetName(),
		InnerCategoryDetail->SectionDetail->GetName());
	TSharedPtr<FDetailTreeNode> DetailTreeNode = SettingDetail.Pin();
	if (!SettingDetail.IsValid())
	{
		if (!FPropertyHolder::Get().UpdateTreeNodes(AsShared(), InnerCategoryDetail))
		{
			SetTextInSearchBox(GetSDetailsView(), GetDisplayName());
			return;
		}
		DetailTreeNode = SettingDetail.Pin();
	}
	DetailTreeNode->SetIsHighlighted(true);
	const TSharedPtr<SDetailTree> DetailTree = GetDetailTree(GetSDetailsView());
	DetailTree->SetItemExpansion(InnerCategoryDetail->CategoryTreeNode.Pin().ToSharedRef(), true);
	DetailTree->RequestScrollIntoView(DetailTreeNode.ToSharedRef());
	GWorld->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (SettingDetail.IsValid())
		{
			SettingDetail.Pin()->SetIsHighlighted(false);
		}
	}, 3.0f, false);
}

FString FPropertyDetail::GetPath() const
{
	return InnerCategoryDetail->GetPath() + Delimiter + GetDisplayName().ToString();
}
