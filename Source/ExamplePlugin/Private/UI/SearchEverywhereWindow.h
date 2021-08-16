#pragma once

#include "Widgets/SWindow.h"

#include "ExamplePluginModule.h"

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

	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath,
	                             const FFocusEvent& InFocusEvent) override;

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	virtual bool SupportsKeyboardFocus() const override;

	TSharedPtr<SSearchEverywhereWidget> GetSearchEverywhereWidget() const;
	TSharedRef<FSearcher> GetSearcher() const;
private:
	FExamplePluginModule& ParentModule = FModuleManager::GetModuleChecked<FExamplePluginModule>(TEXT("ExamplePlugin"));
	TWeakPtr<SWidget> PreviousFocusedWidget;
	TSharedPtr<SSearchEverywhereWidget> InnerWidget;
	bool bNeedToClose = true;
};
