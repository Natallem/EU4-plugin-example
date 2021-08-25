#include "AbstractSettingDetail.h"

#include "CoreMinimal.h"
#include "IDetailsView.h"
#include "PropertyEditor/Private/SDetailsView.h"
#include "PropertyEditor/Private/SDetailsViewBase.h"
#include "SettingsEditor/Private/Widgets/SSettingsEditor.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

DEFINE_LOG_CATEGORY(LogSearchSettingDetail);

template <typename Tag, typename Tag::type M>
struct FHelperPrivateField
{
	friend typename Tag::type get(Tag)
	{
		return M;
	}
};

struct Getter_SettingsView_From_SSettingsEditor
{
	typedef TSharedPtr<IDetailsView> SSettingsEditor::* type;
	friend type get(Getter_SettingsView_From_SSettingsEditor);
};

template struct FHelperPrivateField<Getter_SettingsView_From_SSettingsEditor, &SSettingsEditor::SettingsView>;

struct Getter_CurrentFilter_From_SDetailsViewBase
{
	typedef FDetailFilter SDetailsViewBase::* type;
	friend type get(Getter_CurrentFilter_From_SDetailsViewBase);
};

template struct FHelperPrivateField<Getter_CurrentFilter_From_SDetailsViewBase, &SDetailsViewBase::CurrentFilter>;

struct Getter_DetailTree_From_SDetailsViewBase
{
	typedef TSharedPtr<SDetailTree> SDetailsViewBase::* type;
	friend type get(Getter_DetailTree_From_SDetailsViewBase);
};

template struct FHelperPrivateField<Getter_DetailTree_From_SDetailsViewBase, &SDetailsViewBase::DetailTree>;

struct Getter_SearchBox_From_SDetailsViewBase
{
	typedef TSharedPtr<SSearchBox> SDetailsViewBase::* type;
	friend type get(Getter_SearchBox_From_SDetailsViewBase);
};

template struct FHelperPrivateField<Getter_SearchBox_From_SDetailsViewBase, &SDetailsViewBase::SearchBox>;

FName FAbstractSettingDetail::GetName() const
{
	return FName();
}

TSharedRef<SWidget> FAbstractSettingDetail::GetRowWidget() const
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(GetDisplayName())
		]
		+ SHorizontalBox::Slot()
		  .HAlign(HAlign_Right)
		  .FillWidth(0.25)
		[
			SNew(STextBlock)
    				.ColorAndOpacity(FColor::FromHex("#808080"))
    				.Text(FText::FromString(GetPath()))
		];
}

TSharedPtr<SDetailsView> FAbstractSettingDetail::GetSDetailsView() const
{
	const TSharedPtr<SDockTab> EditorSettingsTab = FGlobalTabmanager::Get()->FindExistingLiveTab(
		FName("EditorSettings"));
	TSharedRef<SWidget> EditorSettingsTabContent = EditorSettingsTab->GetContent();
	const TSharedRef<SSettingsEditor> SettingsEditorWidget = StaticCastSharedRef<SSettingsEditor>(
		EditorSettingsTab->GetContent());
	const TSharedPtr<IDetailsView> DetailsViewPrivateField = (*SettingsEditorWidget).*get(
		Getter_SettingsView_From_SSettingsEditor());
	return StaticCastSharedPtr<SDetailsView>(DetailsViewPrivateField);
}

TSharedPtr<SDetailTree> FAbstractSettingDetail::GetDetailTree(TSharedPtr<SDetailsView> DetailsViewPtr) const
{
	const TSharedPtr<SDetailsViewBase> DetailsViewBasePtr = StaticCastSharedPtr<SDetailsViewBase>(DetailsViewPtr);
	return (*DetailsViewBasePtr).*get(Getter_DetailTree_From_SDetailsViewBase());
}

void FAbstractSettingDetail::SetTextInSearchBox(TSharedPtr<SDetailsView> DetailsViewPtr, const FText& newText)
{
	const TSharedPtr<SDetailsViewBase> DetailsViewBasePtr = StaticCastSharedPtr<SDetailsViewBase>(DetailsViewPtr);
	FDetailFilter& DetailsFilterPrivateField = (*DetailsViewBasePtr).*get(Getter_CurrentFilter_From_SDetailsViewBase());
	DetailsFilterPrivateField.FilterStrings.Empty();
	const TSharedPtr<SSearchBox>& SearchBoxPrivateField = (*DetailsViewBasePtr).*get(
		Getter_SearchBox_From_SDetailsViewBase());
	SearchBoxPrivateField->SetText(newText);
	DetailsViewPtr->RerunCurrentFilter();
}
