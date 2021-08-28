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

	/** This method uses to detect that user clicked outside plugin window border and it's necessary to close plugin window */
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath,
	                             const FFocusEvent& InFocusEvent) override;

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	virtual bool SupportsKeyboardFocus() const override;

	TSharedPtr<SSearchEverywhereWidget> GetSearchEverywhereWidget() const;
	TSharedRef<FSearcher> GetSearcher() const;
private:
	FExamplePluginModule& PluginModule = FModuleManager::GetModuleChecked<FExamplePluginModule>(TEXT("ExamplePlugin"));

	/** WeakPtr to previous focused window to check if user input key can be handled by that window, if command is
	 *cannot be processed by plugin window. */
	TWeakPtr<SWidget> PreviousFocusedWidget;
	TSharedPtr<SSearchEverywhereWidget> InnerWidget;
	bool bNeedToClose = true;
};
