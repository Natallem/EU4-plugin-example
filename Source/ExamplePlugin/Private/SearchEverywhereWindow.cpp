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
	const float ScaleWindow = 0.1f;
	const float MinWidth = 500;
	const float MinHeight = 400;

	// size of window should be a multiple of 10 todo 
	const float PartialParentWidth = round(ParentScreenSize.X / ParentDPIScaleFactor * ScaleWindow / 10.f) * 10;

	// size of window should be a multiple of 10 todo
	const float PartialParentHeight = round(ParentScreenSize.Y / ParentDPIScaleFactor * ScaleWindow / 10.f) * 10;

	const FVector2D WindowSize = FVector2D(FMath::Max(PartialParentWidth, MinWidth),
	                                       FMath::Max(PartialParentHeight, MinHeight));
	SWindow::Construct(
		SWindow::FArguments()
		.Type(EWindowType::Normal)
		.ClientSize(WindowSize)
		// .ClientSize(FVector2D(640, 480))
		.Style(InArgs._Style)
		.MinHeight(MinHeight)
		.MinWidth(MinWidth)
		.IsPopupWindow(true) // if true isn't shown in task bar
		.AutoCenter(EAutoCenter::PrimaryWorkArea)
		// .AutoCenter(EAutoCenter::None)
		[
			SNew(SSearchEverywhereWidget)
		]
	);
	SetAllowFastUpdate(true);
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
	// RequestDestroyWindow();
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
		if (PluginCommandList->ProcessCommandBindings(InKeyEvent))
		{
			return FReply::Handled();
		}
		
		if (PreviousFocusedWidget.IsValid() && PreviousFocusedWidget.Pin().IsValid())  // Q need to create shrd_ptr in var
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

/*
FReply SSearchEverywhereWindow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	/*TArray<TSharedPtr<FBindingContext>> Contexts;
	FInputBindingManager::Get().GetKnownInputContexts(Contexts);
	const FInputChord CheckChord( InKeyEvent.GetKey(), InKeyEvent.IsControlDown(), InKeyEvent.IsAltDown(), InKeyEvent.IsShiftDown(), InKeyEvent.IsCommandDown() );;
	TSharedRef<FUICommandList>& t = IMainFrameModule::Get().GetMainFrameCommandBindings();

	for (TSharedPtr<FBindingContext>& ptr : Contexts)
	{
		const TSharedPtr<FUICommandInfo> Command = FInputBindingManager::Get().FindCommandInContext(
			ptr->GetContextName(), CheckChord, false);
		/*if (ptrr != nullptr)
		{
			
			RequestDestroyWindow();
			return FReply::Unhandled();
		}#2#
		// FMainFrameCommands
		
		
		if( Command.IsValid() && Command->HasActiveChord(CheckChord)  )
		{
			// Find the bound action for this command
			// const FUIAction* Action = FUICommandList::Get().GetActionForCommand(Command);
			const FUIAction* Action = t->GetActionForCommand(Command);
			// If there is no Action mapped to this command list, continue to the next context
			if( Action )
			{
				if(Action->CanExecute())
				{
					// If the action was found and can be executed, do so now
					bool res = Action->Execute();
					RequestDestroyWindow();
					return FReply::Handled();
				}
				else
				{
					// An action wasn't bound to the command but a chord was found or an action was found but cant be executed
					
				}
			}
		}
	}#1#

	return SWindow::OnKeyDown(MyGeometry, InKeyEvent).IsEventHandled();/*)
	{
		RequestDestroyWindow();#2#
		return FReply::Handled();#1#
	/*}
	return FReply::Unhandled();#1#
}
*/

FReply SSearchEverywhereWindow::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	return SWindow::OnKeyUp(MyGeometry, InKeyEvent);
}
