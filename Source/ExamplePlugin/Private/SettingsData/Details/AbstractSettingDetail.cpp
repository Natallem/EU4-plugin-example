#include "AbstractSettingDetail.h"

#include "CoreMinimal.h"
#include "Widgets/SBoxPanel.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"


#include "IDetailsView.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SSearchBox.h"
#include "Framework/Docking/TabManager.h"
#include "SettingsEditor/Private/Widgets/SSettingsEditor.h"
#include "PropertyPath.h"
#include "PropertyEditor/Private/SDetailsView.h"
#include "PropertyEditor/Private/SDetailsViewBase.h"

template <typename Tag, typename Tag::type M>
struct Rob
{
	friend typename Tag::type get(Tag)
	{
		return M;
	}
};

// tag used to access SSettingsEditor::PrivateField
struct Getter_SettingsView_From_SSettingsEditor
{
	typedef TSharedPtr<IDetailsView> SSettingsEditor::* type;
	friend type get(Getter_SettingsView_From_SSettingsEditor);
};

struct Getter_CurrentFilter_From_SDetailsViewBase
{
	typedef FDetailFilter SDetailsViewBase::* type;
	friend type get(Getter_CurrentFilter_From_SDetailsViewBase);
};

struct Getter_SearchBox_From_SDetailsViewBase
{
	typedef TSharedPtr<SSearchBox> SDetailsViewBase::* type;
	friend type get(Getter_SearchBox_From_SDetailsViewBase);
};

template struct Rob<Getter_SettingsView_From_SSettingsEditor, &SSettingsEditor::SettingsView>;
template struct Rob<Getter_CurrentFilter_From_SDetailsViewBase, &SDetailsViewBase::CurrentFilter>;
template struct Rob<Getter_SearchBox_From_SDetailsViewBase, &SDetailsViewBase::SearchBox>;

TSharedRef<SWidget> FAbstractSettingDetail::GetRowWidget() const
{
	if (!RowWidget.IsValid())
	{
		RowWidget = CreateRowWidget();
	}
	return RowWidget.ToSharedRef();
}

TSharedPtr<SWidget> FAbstractSettingDetail::CreateRowWidget() const
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

void FAbstractSettingDetail::Foo() const
{
	const TSharedPtr<SDockTab> EditorSettingsTab = FGlobalTabmanager::Get()->FindExistingLiveTab(
		FName("EditorSettings"));
	TSharedRef<SWidget> EditorSettingsTabContent = EditorSettingsTab->GetContent();
	const TSharedRef<SSettingsEditor> SettingsEditorWidget = StaticCastSharedRef<SSettingsEditor>(
		EditorSettingsTab->GetContent());
	const TSharedPtr<IDetailsView> DetailsViewPrivateField = (*SettingsEditorWidget).*get(
		Getter_SettingsView_From_SSettingsEditor());
	SetTextInSearchBox(StaticCastSharedPtr<SDetailsView>(DetailsViewPrivateField), GetDisplayName());
}


void FAbstractSettingDetail::SetTextInSearchBox(const TSharedPtr<SDetailsView> DetailsViewPtr, const FText& newText) const
{
	const TSharedPtr<SDetailsViewBase> DetailsViewBasePtr = StaticCastSharedPtr<SDetailsViewBase>(DetailsViewPtr);
	FDetailFilter& DetailsFilterPrivateField = (*DetailsViewBasePtr).*get(Getter_CurrentFilter_From_SDetailsViewBase());
	DetailsFilterPrivateField.FilterStrings.Empty();
	const TSharedPtr<SSearchBox>& SearchBoxPrivateField = (*DetailsViewBasePtr).*get(Getter_SearchBox_From_SDetailsViewBase());
	SearchBoxPrivateField->SetText(newText);
	DetailsViewPtr->RerunCurrentFilter();
}
