#pragma once

#include "Widgets/Text/SRichTextBlock.h"

class SSearchEverywhereWidget final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SSearchEverywhereWidget ){}
	SLATE_END_ARGS()

	/**
	 * Construct the widget
	 *
	 * @param InArgs   Declaration from which to construct the widget
	 */
	void Construct(const FArguments& InArgs);

	void FocusDefaultWidget() const;

	void InlineEditableTextCommited(const FText& NewText, ETextCommit::Type CommitType );

	void OnNumericInputTextChanged( const FText& NewText );

	TSharedPtr<SWidget> OnCustomContextMenuOpening();

	TSharedPtr<SWidget> OnDisabledContextMenuOpening();

	bool IsPassword() const;

	ECheckBoxState GetPasswordCheckState() const;

	void OnPasswordCheckStateChanged(ECheckBoxState NewState);

	void ClearSearchBox() const;

	FSlateColor GetLoopingColor() const;

	FReply LaunchPopUp_OnClicked ();

	void OnPopupTextChanged (const FText& NewText);

	void OnPopupTextCommitted( const FText& NewText, ETextCommit::Type CommitInfo ) const;
protected:

	TSharedPtr< SEditableText > EditableText;

	TSharedPtr<SEditableTextBox> SearchBox;

	FCurveSequence Animation;

	TSharedPtr<SErrorText> ErrorText;

	TSharedPtr<SEditableTextBox> NumericInput;

#if WITH_FANCY_TEXT

	TSharedPtr<SRichTextBlock> RichTextBlock;

#endif //WITH_FANCY_TEXT

	TSharedPtr<STextEntryPopup> PopupInput;

	TWeakPtr<IMenu> PopupMenu;

	TSharedPtr<SInlineEditableTextBlock> InlineEditableTextBlock;
	FText InlineEditableText;

	bool bIsPassword;
 };

