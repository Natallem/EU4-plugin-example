#include "SearchEverywhereWindow.h"

#include "SearchEverywhereWidget.h"
#include "Interfaces/IMainFrameModule.h"

void SSearchEverywhereWindow::Construct(const FArguments& InArgs)
{
	SWindow::Construct(
		SWindow::FArguments()
		.ClientSize(FVector2D(640, 480))
		.Style(InArgs._Style)
		.MinHeight(200)
		.MinWidth(500)
		// .ClientSize(FVector2D(640, 480))
		.AutoCenter(EAutoCenter::PrimaryWorkArea)
		.FocusWhenFirstShown(true) // default
		// .UseOSWindowBorder(true)
		.CreateTitleBar(false)
		.ActivationPolicy(EWindowActivationPolicy::Always) // default
		// .IsInitiallyMaximized(true)
		[
			SNew(SSearchEverywhereWidget)
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
	SWindow::OnFocusLost(InFocusEvent); // todo need to call?
	UE_LOG(LogTemp, Log, TEXT("EP : Lost focus"));
	// RequestDestroyWindow();
}

FReply SSearchEverywhereWindow::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	int x = 10;
	
	return FReply::Handled();
}

FReply SSearchEverywhereWindow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	TArray<TSharedPtr<FBindingContext>> Contexts;
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
		}*/
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
	}

	/*if (SWindow::OnKeyDown(MyGeometry, InKeyEvent).IsEventHandled())
	{
		RequestDestroyWindow();*/
		return FReply::Handled();
	/*}
	return FReply::Unhandled();*/
}

FReply SSearchEverywhereWindow::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	return SWindow::OnKeyUp(MyGeometry, InKeyEvent);
}

