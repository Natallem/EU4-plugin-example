#pragma once
#include "ExamplePluginModule.h"
#include "Widgets/SWindow.h"

class FUICommandList;

class SSearchEverywhereWidget;

class SSearchEverywhereWindow final : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SSearchEverywhereWindow)
			: _Style(&FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window"))
			  , _PreviousSearchRequest()
		{
		}

		/** Style used to draw this window */
		SLATE_STYLE_ARGUMENT(FWindowStyle, Style)

		/** Search request in last closed SearchEverywhereWindow */
		SLATE_ARGUMENT(FText, PreviousSearchRequest)
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs, TWeakPtr<SWidget> InPreviousFocusedWidget);

	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath,
	                             const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;

	// todo delete, do nothing
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	virtual bool SupportsKeyboardFocus() const override;

	TSharedPtr<SSearchEverywhereWidget> GetSearchEverywhereWidget() const;
	TSharedRef<FSearcher> GetSearcher() const;
private:
	FExamplePluginModule& ParentModule = FModuleManager::GetModuleChecked<FExamplePluginModule>(TEXT("ExamplePlugin"));
	TWeakPtr<SWidget> PreviousFocusedWidget;
	TSharedPtr<SSearchEverywhereWidget> InnerWidget;
	bool bNeedToClose = true;
};
