#include "SearchEverywhereWindow.h"

#include "SearchEverywhereWidget.h"

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
	SWindow::OnFocusLost(InFocusEvent);// todo need to call?
	UE_LOG(LogTemp, Log, TEXT("EP : Lost focus"));
	RequestDestroyWindow();
}


