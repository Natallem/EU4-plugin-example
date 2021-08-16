#include "SearchEverywhereWindow.h"

#include "Interfaces/IMainFrameModule.h"
#include "Layout/WidgetPath.h"

#include "SearchEverywhereWidget.h"

void SSearchEverywhereWindow::Construct(const FArguments& InArgs,
                                        TWeakPtr<SWidget> InPreviousFocusedWidget)
{
	PreviousFocusedWidget = InPreviousFocusedWidget;

	const TSharedPtr<SWindow> MainFrameWindow = IMainFrameModule::Get().GetParentWindow();
	const FVector2D ParentScreenSize = MainFrameWindow->GetSizeInScreen();
	const float ParentDPIScaleFactor = MainFrameWindow->GetDPIScaleFactor();
	constexpr float ScaleWindow = 0.75f;
	constexpr float MinWidth = 500;
	constexpr float MinHeight = 400;
	const float PartialParentWidth = round(ParentScreenSize.X / ParentDPIScaleFactor * ScaleWindow);
	const float PartialParentHeight = round(ParentScreenSize.Y / ParentDPIScaleFactor * ScaleWindow);
	const FVector2D WindowSize = FVector2D(FMath::Max(PartialParentWidth, MinWidth),
	                                       FMath::Max(PartialParentHeight, MinHeight));

	SWindow::Construct(
		SWindow::FArguments()
		.Type(EWindowType::Normal)
		.ClientSize(WindowSize)
		.Style(InArgs._Style)
		.MinHeight(MinHeight)
		.MinWidth(MinWidth)
		.IsPopupWindow(true) // if true isn't shown in task bar
		.FocusWhenFirstShown(true)
		.AutoCenter(EAutoCenter::PrimaryWorkArea)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1)
			[
				SAssignNew(InnerWidget, SSearchEverywhereWidget, SharedThis(this))
				.PreviousSearchRequest(InArgs._PreviousSearchRequest)
			]
		]
	);
	SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>& Window)
	{
		ParentModule.PreviousSearchRequest = InnerWidget->GetCurrentSearchRequest();
	}));
}

void SSearchEverywhereWindow::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath,
                                              const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	if (!NewWidgetPath.ContainsWidget(SharedThis(this)))
	{
		RequestDestroyWindow();
	}
}

FReply SSearchEverywhereWindow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (!SWindow::OnKeyDown(MyGeometry, InKeyEvent).IsEventHandled())
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			RequestDestroyWindow();
			return FReply::Handled();
		}
		if (ParentModule.PluginCommands->ProcessCommandBindings(InKeyEvent))
		{
			return FReply::Handled();
		}
		if (const TSharedPtr<SWidget> ValidPreviousFocusedWidget = PreviousFocusedWidget.Pin())
		{
			if (ValidPreviousFocusedWidget->OnKeyDown(MyGeometry, InKeyEvent).IsEventHandled())
			{
				RequestDestroyWindow();
				return FReply::Handled();
			}
		}
		return FReply::Unhandled();
	}
	return FReply::Handled();
}

bool SSearchEverywhereWindow::SupportsKeyboardFocus() const
{
	return InnerWidget->SupportsKeyboardFocus();
}

TSharedPtr<SSearchEverywhereWidget> SSearchEverywhereWindow::GetSearchEverywhereWidget() const
{
	return InnerWidget;
}

TSharedRef<FSearcher> SSearchEverywhereWindow::GetSearcher() const
{
	return ParentModule.Searcher;
}
