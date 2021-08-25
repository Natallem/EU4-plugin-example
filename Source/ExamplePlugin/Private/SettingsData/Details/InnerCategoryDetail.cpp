#include "InnerCategoryDetail.h"

#include "ISettingsModule.h"

#include "CategoryDetail.h"
#include "SectionDetail.h"

FInnerCategoryDetail::FInnerCategoryDetail(const TSharedRef<FSectionDetail>& SectionDetail,
                                           const FText& InnerCategoryDisplayName,
                                           const TWeakPtr<FDetailCategoryImpl>& CategoryTreeNode, int SettingsIndex)
	: SectionDetail(SectionDetail),
	  CategoryTreeNode(CategoryTreeNode),
	  InnerCategoryDisplayName(InnerCategoryDisplayName),
	  SettingsIndex(SettingsIndex)
{
}

FText FInnerCategoryDetail::GetDisplayName() const
{
	return InnerCategoryDisplayName;
}

void FInnerCategoryDetail::DoAction()
{
	SectionDetail->CategoryDetail->SettingsModule.ShowViewer(
		FName("Editor"),
		SectionDetail->CategoryDetail->GetName(),
		SectionDetail->GetName());
	SetTextInSearchBox(GetSDetailsView(), GetDisplayName());
}

FString FInnerCategoryDetail::GetPath() const
{
	return SectionDetail->GetPath() + Delimiter + GetDisplayName().ToString();
}
