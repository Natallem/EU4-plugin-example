#include "SearchEverywhereWidget.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/STextEntryPopup.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void SSearchEverywhereWidget::Construct(const FArguments& InArgs)
{
	bIsPassword = true;

	InlineEditableText = LOCTEXT( "TestingInlineEditableTextBlock", "Testing inline editable text block!" );

	Animation = FCurveSequence(0, 5);
		
	Animation.Play(this->AsShared(), true);

	this->ChildSlot
	[
		SNew( SScrollBox )
		+SScrollBox::Slot()
		[
			SNew( SVerticalBox )
			+ SVerticalBox::Slot().AutoHeight() .HAlign(HAlign_Center) .Padding(5)			
			[
				SAssignNew( EditableText, SEditableText )
					.Text( LOCTEXT( "TestingTextControl", "Testing editable text control (no box)" ) )
					.Font( FCoreStyle::GetDefaultFontStyle("Regular", 12) )
					.HintText(LOCTEXT("TestingTextControlHint", "Hint Text"))
			]
			+ SVerticalBox::Slot().AutoHeight() .HAlign(HAlign_Center) .Padding(5)
			[
				SNew( SEditableTextBox )
					.Text( LOCTEXT( "TestingReadOnlyTextBox", "Read only editable text box (with tool tip!)" ) )
					.Font( FCoreStyle::GetDefaultFontStyle("Regular", 20) )
					.IsReadOnly( true )
					.ToolTipText( LOCTEXT("TestingReadOnlyTextBox_Tooltip", "Testing tool tip for editable text!") )
					.HintText(LOCTEXT("TestingReadOnlyTextBoxHint", "Hint Text") )
			]
			+ SVerticalBox::Slot().AutoHeight() .HAlign(HAlign_Center) .Padding(5)
			[
				SNew( SEditableTextBox )
					.Text( LOCTEXT( "TestingLongText", "Here is an editable text box with a very long initial string.  Useful to test scrolling.  Remember, this editable text box has many features, such as cursor navigation, text selection with either the mouse or keyboard, and cut, copy and paste.  You can even undo and redo just how you'd expect to." ) )
					.Font( FCoreStyle::GetDefaultFontStyle("Bold", 13) )
					.HintText(LOCTEXT("TestingLongTextHint", "Hint Text"))
			]
			+ SVerticalBox::Slot().AutoHeight() .HAlign(HAlign_Center) .Padding(5)
			[
				SNew( SEditableTextBox )
					.Text( LOCTEXT( "TestingBigTextBigMargin", "Big text, big margin!" ) )
					.Font( FCoreStyle::GetDefaultFontStyle("Bold", 40) )
					.RevertTextOnEscape( true )
					.BackgroundColor( this, &SSearchEverywhereWidget::GetLoopingColor )
					.HintText(LOCTEXT("TestingBigTextMarginHint", "Hint Text"))
			]
			+ SVerticalBox::Slot().AutoHeight() .HAlign(HAlign_Center) .Padding(5)			
			[
				SAssignNew(InlineEditableTextBlock, SInlineEditableTextBlock)
					.Font( FCoreStyle::GetDefaultFontStyle("Regular", 12) )
					.Text( InlineEditableText )
					.OnTextCommitted( this, &SSearchEverywhereWidget::InlineEditableTextCommited )
					.ToolTipText( LOCTEXT("TestingInlineEditableTextBlock_Tooltip", "Testing tool tip for inline editable text block!") )
			]
			+ SVerticalBox::Slot().AutoHeight() .HAlign(HAlign_Center) .VAlign(VAlign_Center) .Padding( 5 )
			[
				SAssignNew( SearchBox, SSearchBox )
				//.Text( LOCTEXT("TestingSearchBox", "Testing search boxes tool tip") )
				//.Font( FCoreStyle::GetDefaultFontStyle("Bold", 12) )
				.ToolTipText( LOCTEXT("TestingSearchBox_Tooltip", "Testing search boxes") )
			]
			+ SVerticalBox::Slot().AutoHeight() .HAlign(HAlign_Center) .VAlign(VAlign_Center) .Padding( 5 )
			[
				SAssignNew( NumericInput, SEditableTextBox )
					.Text( LOCTEXT( "NumericInput", "This should be a number" ) )
					.OnTextChanged( this, &SSearchEverywhereWidget::OnNumericInputTextChanged )
					.RevertTextOnEscape( true )
					.HintText(LOCTEXT("NumericInputHint", "Enter a number"))
			]
			+ SVerticalBox::Slot().AutoHeight() .HAlign(HAlign_Center) .VAlign(VAlign_Center) .Padding( 5 )
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .AutoWidth()
				  . HAlign(HAlign_Center)
				[
					SNew(SButton)
								.Text( LOCTEXT("PopupTest", "PopUp Test") )
								.OnClicked( this, &SSearchEverywhereWidget::LaunchPopUp_OnClicked )
				]
			]
			+SVerticalBox::Slot().AutoHeight() .VAlign(VAlign_Bottom) .Padding(0,20,0,0)
			[
				SAssignNew(ErrorText, SErrorText)
			]
			+ SVerticalBox::Slot().AutoHeight() .HAlign(HAlign_Center) .VAlign(VAlign_Center) .Padding( 5 )
			[
				SNew( SEditableTextBox )
					.Text( LOCTEXT( "CustomContextMenuInput", "This text box has a custom context menu" ) )
					.RevertTextOnEscape( true )
					.HintText(LOCTEXT("CustomContextMenuHint", "Custom context menu..."))
					.OnContextMenuOpening( this, &SSearchEverywhereWidget::OnCustomContextMenuOpening )
			]
			+ SVerticalBox::Slot().AutoHeight() .HAlign(HAlign_Center) .VAlign(VAlign_Center) .Padding( 5 )
			[
				SNew( SEditableTextBox )
					.Text( LOCTEXT( "DisabledContextMenuInput", "This text box has no context menu" ) )
					.RevertTextOnEscape( true )
					.HintText(LOCTEXT("DisabledContextMenuHint", "No context menu..."))
					.OnContextMenuOpening(this, &SSearchEverywhereWidget::OnDisabledContextMenuOpening)
			]
			+ SVerticalBox::Slot().AutoHeight() .HAlign(HAlign_Center) .VAlign(VAlign_Center) .Padding( 5 )
			[
				SNew(SHorizontalBox)

				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SEditableTextBox)
						.IsPassword(this, &SSearchEverywhereWidget::IsPassword)
						.RevertTextOnEscape(true)
						.MinDesiredWidth(400)
						.HintText(LOCTEXT("EditablePasswordHintText", "This text box can be a password"))
				]

				+SHorizontalBox::Slot()
				 .AutoWidth()
				 .Padding(FMargin(4, 0))
				[
					SNew(SCheckBox)
						.IsChecked(this, &SSearchEverywhereWidget::GetPasswordCheckState)
						.OnCheckStateChanged(this, &SSearchEverywhereWidget::OnPasswordCheckStateChanged)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("PasswordCheckBoxText", "Password?"))
					]
				]
			]
		]
	];
}

void SSearchEverywhereWidget::FocusDefaultWidget() const
{
	// Set focus to the editable text, so the user doesn't have to click initially to type
	FWidgetPath WidgetToFocusPath;
	FSlateApplication::Get().GeneratePathToWidgetUnchecked( EditableText.ToSharedRef(), WidgetToFocusPath );
	FSlateApplication::Get().SetKeyboardFocus( WidgetToFocusPath, EFocusCause::SetDirectly );
}

void SSearchEverywhereWidget::InlineEditableTextCommited(const FText& NewText, ETextCommit::Type CommitType)
{
	InlineEditableText = NewText;
	InlineEditableTextBlock->SetText( InlineEditableText );
}

void SSearchEverywhereWidget::OnNumericInputTextChanged(const FText& NewText)
{
	const FText Error = (NewText.IsNumeric())
		                    ? FText::GetEmpty()
		                    : FText::Format( LOCTEXT("NotANumberWarning", "'{0}' is not a number"), NewText );

	ErrorText->SetError( Error );
	NumericInput->SetError( Error );
}

TSharedPtr<SWidget> SSearchEverywhereWidget::OnCustomContextMenuOpening()
{
	return SNew(SBorder)
			.Padding(5.0f)
			.BorderImage(FCoreStyle::Get().GetBrush("BoxShadow"))
	[
		SNew(STextBlock)
		.Text(LOCTEXT("CustomContextMenuContent", "This context menu content is just a text block"))
	];
}

TSharedPtr<SWidget> SSearchEverywhereWidget::OnDisabledContextMenuOpening()
{
	return TSharedPtr<SWidget>();
}

bool SSearchEverywhereWidget::IsPassword() const
{
	return bIsPassword;
}

ECheckBoxState SSearchEverywhereWidget::GetPasswordCheckState() const
{
	return bIsPassword ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SSearchEverywhereWidget::OnPasswordCheckStateChanged(ECheckBoxState NewState)
{
	bIsPassword = NewState == ECheckBoxState::Checked;
}

void SSearchEverywhereWidget::ClearSearchBox() const
{
	SearchBox->SetText( FText::GetEmpty() );
}

FSlateColor SSearchEverywhereWidget::GetLoopingColor() const
{
	return FLinearColor( 360*Animation.GetLerp(), 0.8f, 1.0f).HSVToLinearRGB();
}

FReply SSearchEverywhereWidget::LaunchPopUp_OnClicked()
{		
	FText DefaultText( LOCTEXT("EnterThreeChars", "Enter a three character string") );

	TSharedRef<STextEntryPopup> TextEntry = SAssignNew( PopupInput, STextEntryPopup )
		.Label( DefaultText )
		.ClearKeyboardFocusOnCommit ( false )
		.OnTextChanged( this, &SSearchEverywhereWidget::OnPopupTextChanged )
		.OnTextCommitted( this, &SSearchEverywhereWidget::OnPopupTextCommitted ) 
		.HintText( DefaultText );

	PopupMenu = FSlateApplication::Get().PushMenu(
		AsShared(), // Parent widget should be TestSyuite, not the menu thats open or it will be closed when the menu is dismissed
		FWidgetPath(),
		TextEntry,
		FSlateApplication::Get().GetCursorPos(), // summon location
		FPopupTransitionEffect( FPopupTransitionEffect::TypeInPopup )
	);

	return FReply::Handled();
}

void SSearchEverywhereWidget::OnPopupTextChanged(const FText& NewText)
{
	const FText Error = ( NewText.ToString().Len() == 3 )
		                    ? FText::GetEmpty()
		                    : FText::Format( LOCTEXT("ThreeCharsError", "'{0}' is not three characters"), NewText);

	ErrorText->SetError( Error );
	PopupInput->SetError( Error );
}

void SSearchEverywhereWidget::OnPopupTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo) const
{
	if ( (CommitInfo == ETextCommit::OnEnter) && (NewText.ToString().Len() == 3) )
	{
		// manually close menu on validated committal
		if (PopupMenu.IsValid())
		{		
			PopupMenu.Pin()->Dismiss();
		}
	}
}


#undef LOCTEXT_NAMESPACE
