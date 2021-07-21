#include "SearchEverywhereWindow.h"

#include "SearchEverywhereWidget.h"
#include "Interfaces/IMainFrameModule.h"
#include "Physics/Experimental/ChaosInterfaceWrapper.h"

void SSearchEverywhereWindow::Construct(const FArguments& InArgs, TWeakPtr<SWidget> NewPreviousFocusedWidget,
                                        TSharedPtr<FUICommandList> NewPluginCommandList)
{
	PreviousFocusedWidget = NewPreviousFocusedWidget;
	PluginCommandList = NewPluginCommandList;
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWindow Construct"));
	const TSharedPtr<SWindow> MainFrameWindow = IMainFrameModule::Get().GetParentWindow();
	const FVector2D ParentScreenSize = MainFrameWindow->GetSizeInScreen();
	const float ParentDPIScaleFactor = MainFrameWindow->GetDPIScaleFactor();
	const float ScaleWindow = 0.75f;
	const float MinWidth = 500;
	const float MinHeight = 400;

	// size of window should be a multiple of 10 todo 
	const float PartialParentWidth = round(ParentScreenSize.X / ParentDPIScaleFactor * ScaleWindow);

	// size of window should be a multiple of 10 todo
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
				SAssignNew(InnerWidget, SSearchEverywhereWidget)
			]
		]
	);
	SetOnWindowClosed(FOnWindowClosed::CreateLambda([](const TSharedRef<SWindow>& Window)
	{
		// todo save state before close
		UE_LOG(LogTemp, Log, TEXT("EP : Lambda before close"));
	}));
}


void SSearchEverywhereWindow::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWindow OnFocusLost"));
	SWindow::OnFocusLost(InFocusEvent);
	if (bNeedToClose)
	{
		RequestDestroyWindow();
	}
}

void SSearchEverywhereWindow::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath,
                                              const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	SWindow::OnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
	if (NewWidgetPath.ContainsWidget(InnerWidget.ToSharedRef()) || NewWidgetPath.ContainsWidget(SharedThis(this)))
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

FReply SSearchEverywhereWindow::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Log, TEXT("EP : SSearchEverywhereWindow OnFocusReceived"));
	return FReply::Handled();
}

FReply SSearchEverywhereWindow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (!SWindow::OnKeyDown(MyGeometry, InKeyEvent).IsEventHandled())
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			RequestDestroyWindow();
		}
		if (PluginCommandList->ProcessCommandBindings(InKeyEvent))
		{
			return FReply::Handled();
		}

		if (PreviousFocusedWidget.IsValid() && PreviousFocusedWidget.Pin().IsValid())
			// Q need to create shrd_ptr in var
		{
			if (PreviousFocusedWidget.Pin()->OnKeyDown(MyGeometry, InKeyEvent).IsEventHandled())
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

void SSearchEverywhereWindow::OnNewDataFound()
{
	int x = 10;
}
