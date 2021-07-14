#include "SearchEverywhereWidget.h"

#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Programs/UnrealLightmass/Private/ImportExport/3DVisualizer.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/STextEntryPopup.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void SSearchEverywhereWidget::Construct(const FArguments& InArgs)
{
	// bIsPassword = true;

	// InlineEditableText = LOCTEXT( "TestingInlineEditableTextBlock", "Testing inline editable text block!" );

	// Animation = FCurveSequence(0, 5);

	// Animation.Play(this->AsShared(), true);
	ChildSlot
	[
		SNew(SBox)
			// .WidthOverride(600)
			// .HeightOverride(400)
			
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			
			[
				SAssignNew(EditableTextBox, SEditableTextBox)
				.Text(LOCTEXT("DisabledContextMenuInput", "This text box has no context menu"))
				.RevertTextOnEscape(true)
				.OnTextChanged(this, &SSearchEverywhereWidget::OnTextChanged)
				// .HintText(LOCTEXT("DisabledContextMenuHint", "No context menu..."))
				// .OnContextMenuOpening(this, &SSearchEverywhereWidget::OnDisabledContextMenuOpening)
			]
		]
	];
	
	/*this->ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).VAlign(VAlign_Center).Padding(5)
			[
				SAssignNew(EditableTextBox, SEditableTextBox)
					.Text(LOCTEXT("DisabledContextMenuInput", "This text box has no context menu"))
					.RevertTextOnEscape(true)
					.OnTextChanged(this, &SSearchEverywhereWidget::OnTextChanged)
					.HintText(LOCTEXT("DisabledContextMenuHint", "No context menu..."))
					.OnContextMenuOpening(this, &SSearchEverywhereWidget::OnDisabledContextMenuOpening)
			]
		]
	];*/
}

FReply SSearchEverywhereWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

void SSearchEverywhereWidget::OnTextChanged(const FText& Filter)
{
	int x = 10;
	EditableTextBox->SetText(Filter);
}

TOptional<EFocusCause> SSearchEverywhereWidget::HasInsideUserFocus(uint32 User)
{
	return EditableText->HasUserFocus(User);
}

void SSearchEverywhereWidget::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWidget OnFocusLost"));

	SCompoundWidget::OnFocusLost(InFocusEvent);
}

FReply SSearchEverywhereWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWidget OnFocusReceived"));

	return SCompoundWidget::OnFocusReceived(MyGeometry, InFocusEvent);
}

void SSearchEverywhereWidget::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath,
	const FWidgetPath& NewWidgetPath)
{
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWidget OnFocusChanging"));

	SCompoundWidget::OnFocusChanging(PreviousFocusPath, NewWidgetPath);
}


#undef LOCTEXT_NAMESPACE
