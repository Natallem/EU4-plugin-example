#include "SearchEverywhereWindow.h"

#include "SearchEverywhereWidget.h"
#include "Interfaces/IMainFrameModule.h"
#include "Layout/WidgetPath.h"


void SSearchEverywhereWindow::Construct(const FArguments& InArgs,
                                        TWeakPtr<SWidget> InPreviousFocusedWidget)
{
	PreviousFocusedWidget = InPreviousFocusedWidget;

	const TSharedPtr<SWindow> MainFrameWindow = IMainFrameModule::Get().GetParentWindow();
	const FVector2D ParentScreenSize = MainFrameWindow->GetSizeInScreen();
	const float ParentDPIScaleFactor = MainFrameWindow->GetDPIScaleFactor();
	const float ScaleWindow = 0.75f;
	const float MinWidth = 500;
	const float MinHeight = 400;
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


FReply SSearchEverywhereWindow::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWindow OnFocusReceived"));
	return FReply::Handled();
}


void SSearchEverywhereWindow::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath,
                                              const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	FString t1 = PreviousFocusPath.ToWidgetPathRef()->ToString();
	FString t2 = NewWidgetPath.ToString();

	// UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWindow OnFocusChanging PrevPath: %s NewPat %s"), *t1, *t2);
	SWindow::OnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
	if (NewWidgetPath.ContainsWidget(SharedThis(this)))
		// todo change only for this window in path
	{
		bNeedToClose = false;
		UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWindow OnFocusChanging contains inner"));
	}
	else
	{
		bNeedToClose = true;
		UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWindow OnFocusChanging NOT contains inner"));
	}
	// bNeedToClose = !NewWidgetPath.ContainsWidget(InnerWidget.ToSharedRef());
	if (bNeedToClose)
	{
		RequestDestroyWindow();
	}
}

void SSearchEverywhereWindow::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWindow OnFocusLost"));
	SWindow::OnFocusLost(InFocusEvent);;
	if (bNeedToClose)
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

FReply SSearchEverywhereWindow::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	return SWindow::OnKeyUp(MyGeometry, InKeyEvent);
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
