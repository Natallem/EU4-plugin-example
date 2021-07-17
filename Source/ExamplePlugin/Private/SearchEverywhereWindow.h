#pragma once
#include "SearchEverywhereWidget.h"

enum ESearchModeTab
{
	/** todo comment */
	All,
	/** todo comment */
	Classes,
};

class SSearchEverywhereWindow final : public SWindow
{
public:
	
	SLATE_BEGIN_ARGS(SSearchEverywhereWindow)
			: _Type(ESearchModeTab::All)
			  , _Style(&FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window"))
			  , _SearchRequest()
		{}

		/** Type of this window */
		SLATE_ARGUMENT(ESearchModeTab, Type)

		/** Style used to draw this window */
		SLATE_STYLE_ARGUMENT(FWindowStyle, Style)

		/** Title of the window */
		SLATE_ATTRIBUTE(FText, SearchRequest)
	SLATE_END_ARGS()
	
	TWeakPtr<SWidget> PreviousFocusedWidget;
	TSharedPtr< FUICommandList > PluginCommandList;
	TSharedPtr<SWidget> InnerWidget; // todo specify? to SSearchWindow
	void Construct(const FArguments& InArgs,TWeakPtr<SWidget> NewPreviousFocusedWidget, TSharedPtr< FUICommandList > NewPluginCommandList);
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath,
		const FFocusEvent& InFocusEvent) override;
	
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override; // todo delete, do nothing
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
private:
	bool bNeedToClose = true;
};
