#pragma once

#include "Widgets/Text/SRichTextBlock.h"

class SSearchEverywhereWidget final : public SCompoundWidget
{
public:
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	SLATE_BEGIN_ARGS(SSearchEverywhereWidget) {}
	SLATE_END_ARGS()

	/**
	 * Construct the widget
	 *
	 * @param InArgs   Declaration from which to construct the widget
	 */
	void Construct(const FArguments& InArgs);

	void OnTextChanged(const FText& Filter);
	TOptional<EFocusCause> HasInsideUserFocus(uint32 User);
protected:
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath) override;


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

	bool bIsPassword = false;

	TSharedPtr<class SEditableTextBox> EditableTextBox;
 };

