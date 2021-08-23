#include "SectionDetail.h"

#include "CategoryDetail.h"
#include "ISettingsModule.h"
#include "PropertyEditor/Private/SDetailsView.h"
#include "PropertyPath.h"

FSectionDetail::FSectionDetail(const TSharedRef<FCategoryDetail>& CategoryDetail,
                               const ISettingsSectionPtr& SettingsSection,
                               const TWeakPtr<FDetailMultiTopLevelObjectRootNode>& SettingTreeNode,
                               int SettingIndex)
	: CategoryDetail(CategoryDetail),
	  SettingsSection(SettingsSection),
	  SettingTreeNode(SettingTreeNode),
	  SettingIndex(SettingIndex)
{
	SectionDisplayName = SettingsSection->GetDisplayName();
}

FText FSectionDetail::GetDisplayName() const
{
	return SectionDisplayName;
}

FName FSectionDetail::GetName() const
{
	return SettingsSection->GetName();
}

void FSectionDetail::DoAction()
{
	CategoryDetail->SettingsModule.ShowViewer(FName("Editor"),
	                                          CategoryDetail->GetName(),
	                                          GetName());
}

FString FSectionDetail::GetPath() const
{
	return CategoryDetail->GetPath() + Delimiter + GetDisplayName().ToString();
}
