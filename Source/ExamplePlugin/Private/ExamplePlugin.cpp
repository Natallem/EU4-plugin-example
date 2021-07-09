// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExamplePlugin.h"
#include "ExamplePluginStyle.h"
#include "ExamplePluginCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/STextEntryPopup.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/Text/SRichTextBlock.h"

static const FName ExamplePluginTabName("ExamplePlugin");

#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void FExamplePluginModule::StartupModule()
{
	FExamplePluginStyle::Initialize();
	FExamplePluginStyle::ReloadTextures();

	FExamplePluginCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FExamplePluginCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FExamplePluginModule::PluginButtonClicked),
		FCanExecuteAction());

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ExamplePluginTabName,
	                                                  FOnSpawnTab::CreateRaw(
		                                                  this, &FExamplePluginModule::OnSpawnPluginTab))
	                        // .SetDisplayName(LOCTEXT("FExamplePluginTabTitle", "ExamplePlugin"))
	                        .SetMenuType(ETabSpawnerMenuType::Hidden);

	// Append to level editor module so that shortcuts are accessible in level editor
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditorModule.GetGlobalLevelEditorActions()->Append(PluginCommands.ToSharedRef());
}

void FExamplePluginModule::ShutdownModule()
{
	FExamplePluginStyle::Shutdown();

	FExamplePluginCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ExamplePluginTabName);
}

class STextEditTest : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STextEditTest)
		{
		}

	SLATE_END_ARGS()

	/**
	 * Construct the widget
	 *
	 * @param InArgs   Declaration from which to construct the widget
	 */
	void Construct(const FArguments& InArgs)
	{
		bIsPassword = true;

		InlineEditableText = LOCTEXT("TestingInlineEditableTextBlock", "Testing inline editable text block!");

		Animation = FCurveSequence(0, 5);

		Animation.Play(this->AsShared(), true);

		this->ChildSlot
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(5)
				[
					SAssignNew(EditableText, SEditableText)
					.Text(LOCTEXT("TestingTextControl", "Testing editable text control (no box)"))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
					.HintText(LOCTEXT("TestingTextControlHint", "Hint Text"))
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(5)
				[
					SNew(SEditableTextBox)
					.Text(LOCTEXT("TestingReadOnlyTextBox", "Read only editable text box (with tool tip!)"))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 20))
					.IsReadOnly(true)
					.ToolTipText(LOCTEXT("TestingReadOnlyTextBox_Tooltip", "Testing tool tip for editable text!"))
					.HintText(LOCTEXT("TestingReadOnlyTextBoxHint", "Hint Text"))
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(5)
				[
					SNew(SEditableTextBox)
					.Text(LOCTEXT("TestingLongText",
					              "Here is an editable text box with a very long initial string.  Useful to test scrolling.  Remember, this editable text box has many features, such as cursor navigation, text selection with either the mouse or keyboard, and cut, copy and paste.  You can even undo and redo just how you'd expect to."))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 13))
					.HintText(LOCTEXT("TestingLongTextHint", "Hint Text"))
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(5)
				[
					SNew(SEditableTextBox)
					.Text(LOCTEXT("TestingBigTextBigMargin", "Big text, big margin!"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 40))
					.RevertTextOnEscape(true)
					.BackgroundColor(this, &STextEditTest::GetLoopingColor)
					.HintText(LOCTEXT("TestingBigTextMarginHint", "Hint Text"))
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(5)
				[
					SAssignNew(InlineEditableTextBlock, SInlineEditableTextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
					.Text(InlineEditableText)
					.OnTextCommitted(this, &STextEditTest::InlineEditableTextCommited)
					.ToolTipText(LOCTEXT("TestingInlineEditableTextBlock_Tooltip",
					                     "Testing tool tip for inline editable text block!"))
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).VAlign(VAlign_Center).Padding(5)
				[
					SAssignNew(SearchBox, SSearchBox)
					//.Text( LOCTEXT("TestingSearchBox", "Testing search boxes tool tip") )
					//.Font( FCoreStyle::GetDefaultFontStyle("Bold", 12) )
					.ToolTipText(LOCTEXT("TestingSearchBox_Tooltip", "Testing search boxes"))
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).VAlign(VAlign_Center).Padding(5)
				[
					SAssignNew(NumericInput, SEditableTextBox)
					.Text(LOCTEXT("NumericInput", "This should be a number"))
					.OnTextChanged(this, &STextEditTest::OnNumericInputTextChanged)
					.RevertTextOnEscape(true)
					.HintText(LOCTEXT("NumericInputHint", "Enter a number"))
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).VAlign(VAlign_Center).Padding(5)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					  .AutoWidth()
					  .HAlign(HAlign_Center)
					[
						SNew(SButton)
								.Text(LOCTEXT("PopupTest", "PopUp Test"))
								.OnClicked(this, &STextEditTest::LaunchPopUp_OnClicked)
					]
				]
				+ SVerticalBox::Slot().AutoHeight().VAlign(VAlign_Bottom).Padding(0, 20, 0, 0)
				[
					SAssignNew(ErrorText, SErrorText)
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).VAlign(VAlign_Center).Padding(5)
				[
					SNew(SEditableTextBox)
					.Text(LOCTEXT("CustomContextMenuInput", "This text box has a custom context menu"))
					.RevertTextOnEscape(true)
					.HintText(LOCTEXT("CustomContextMenuHint", "Custom context menu..."))
					.OnContextMenuOpening(this, &STextEditTest::OnCustomContextMenuOpening)
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).VAlign(VAlign_Center).Padding(5)
				[
					SNew(SEditableTextBox)
					.Text(LOCTEXT("DisabledContextMenuInput", "This text box has no context menu"))
					.RevertTextOnEscape(true)
					.HintText(LOCTEXT("DisabledContextMenuHint", "No context menu..."))
					.OnContextMenuOpening(this, &STextEditTest::OnDisabledContextMenuOpening)
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).VAlign(VAlign_Center).Padding(5)
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SEditableTextBox)
						.IsPassword(this, &STextEditTest::IsPassword)
						.RevertTextOnEscape(true)
						.MinDesiredWidth(400)
						.HintText(LOCTEXT("EditablePasswordHintText", "This text box can be a password"))
					]

					+ SHorizontalBox::Slot()
					  .AutoWidth()
					  .Padding(FMargin(4, 0))
					[
						SNew(SCheckBox)
						.IsChecked(this, &STextEditTest::GetPasswordCheckState)
						.OnCheckStateChanged(this, &STextEditTest::OnPasswordCheckStateChanged)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("PasswordCheckBoxText", "Password?"))
						]
					]
				]
			]
		];
	}

	void FocusDefaultWidget()
	{
		// Set focus to the editable text, so the user doesn't have to click initially to type
		FWidgetPath WidgetToFocusPath;
		FSlateApplication::Get().GeneratePathToWidgetUnchecked(EditableText.ToSharedRef(), WidgetToFocusPath);
		FSlateApplication::Get().SetKeyboardFocus(WidgetToFocusPath, EFocusCause::SetDirectly);
	}

	void InlineEditableTextCommited(const FText& NewText, ETextCommit::Type CommitType)
	{
		InlineEditableText = NewText;
		InlineEditableTextBlock->SetText(InlineEditableText);
	}

	void OnNumericInputTextChanged(const FText& NewText)
	{
		const FText Error = (NewText.IsNumeric())
			                    ? FText::GetEmpty()
			                    : FText::Format(LOCTEXT("NotANumberWarning", "'{0}' is not a number"), NewText);

		ErrorText->SetError(Error);
		NumericInput->SetError(Error);
	}

	TSharedPtr<SWidget> OnCustomContextMenuOpening()
	{
		return SNew(SBorder)
			.Padding(5.0f)
			.BorderImage(FCoreStyle::Get().GetBrush("BoxShadow"))
		[
			SNew(STextBlock)
			.Text(LOCTEXT("CustomContextMenuContent", "This context menu content is just a text block"))
		];
	}

	TSharedPtr<SWidget> OnDisabledContextMenuOpening()
	{
		return TSharedPtr<SWidget>();
	}

	bool IsPassword() const
	{
		return bIsPassword;
	}

	ECheckBoxState GetPasswordCheckState() const
	{
		return bIsPassword ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	void OnPasswordCheckStateChanged(ECheckBoxState NewState)
	{
		bIsPassword = NewState == ECheckBoxState::Checked;
	}

	void ClearSearchBox()
	{
		SearchBox->SetText(FText::GetEmpty());
	}

	FSlateColor GetLoopingColor() const
	{
		return FLinearColor(360 * Animation.GetLerp(), 0.8f, 1.0f).HSVToLinearRGB();
	}

	FReply LaunchPopUp_OnClicked()
	{
		FText DefaultText(LOCTEXT("EnterThreeChars", "Enter a three character string"));

		TSharedRef<STextEntryPopup> TextEntry = SAssignNew(PopupInput, STextEntryPopup)
			.Label(DefaultText)
			.ClearKeyboardFocusOnCommit(false)
			.OnTextChanged(this, &STextEditTest::OnPopupTextChanged)
			.OnTextCommitted(this, &STextEditTest::OnPopupTextCommitted)
			.HintText(DefaultText);

		PopupMenu = FSlateApplication::Get().PushMenu(
			AsShared(),
			// Parent widget should be TestSyuite, not the menu thats open or it will be closed when the menu is dismissed
			FWidgetPath(),
			TextEntry,
			FSlateApplication::Get().GetCursorPos(), // summon location
			FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup)
		);

		return FReply::Handled();
	}

	void OnPopupTextChanged(const FText& NewText)
	{
		const FText Error = (NewText.ToString().Len() == 3)
			                    ? FText::GetEmpty()
			                    : FText::Format(LOCTEXT("ThreeCharsError", "'{0}' is not three characters"), NewText);

		ErrorText->SetError(Error);
		PopupInput->SetError(Error);
	}

	void OnPopupTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
	{
		if ((CommitInfo == ETextCommit::OnEnter) && (NewText.ToString().Len() == 3))
		{
			// manually close menu on validated committal
			if (PopupMenu.IsValid())
			{
				PopupMenu.Pin()->Dismiss();
			}
		}
	}

protected:
	TSharedPtr<SEditableText> EditableText;

	TSharedPtr<SEditableTextBox> SearchBox;

	FCurveSequence Animation;

	TSharedPtr<SErrorText> ErrorText;

	TSharedPtr<SEditableTextBox> NumericInput;

#if WITH_FANCY_TEXT

	TSharedPtr<SRichTextBlock> RichTextBlock;

#endif //WITH_FANCY_TEXT

	TSharedPtr<STextEntryPopup> PopupInput;

	TWeakPtr<IMenu> PopupMenu;

	TSharedPtr<SInlineEditableTextBlock> InlineEditableTextBlock;
	FText InlineEditableText;

	bool bIsPassword;
};

TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	/*
	*"AllExampleTab", ETabState::OpenedTab)
	->AddTab("ClassesExampleTab", ETabState::OpenedTab)
	->AddTab("FilesExampleTab", E
	 */
	FText LabelNameText;
	FText ToolTipText;
	if (TabIdentifier == FName("AllExampleTab"))
	{
		LabelNameText = LOCTEXT("AllExampleTab", "All");
		ToolTipText = LOCTEXT("AllExampleTabToolTip", "Shows all content");
	}
	else if (TabIdentifier == FName("ClassesExampleTab"))
	{
		LabelNameText = LOCTEXT("ClassesExampleTab", "Classes");
		ToolTipText = LOCTEXT("ClassesExampleTabToolTip", "Shows classes only");
	}
	else if (TabIdentifier == FName("FilesExampleTab"))
	{
		LabelNameText = LOCTEXT("FilesExampleTab", "Files");
		ToolTipText = LOCTEXT("FilesExampleTabToolTip", "Shows files only");
	}
	else
	{
		ensure(false);
		return SNew(SDockTab);
	}
	return SNew(SDockTab)
			.Label(LabelNameText)
			.ToolTipText(ToolTipText)
			.Clipping(EWidgetClipping::ClipToBounds)
	[
		SNew(STextEditTest)
	];
}

static TSharedPtr<FTabManager> TestSuite1TabManager;

TSharedRef<SDockTab> FExamplePluginModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FExamplePluginModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("ExamplePlugin.cpp"))
	);

	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("TestSuite2_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->Split
			(
				FTabManager::NewStack()
				->AddTab("AllExampleTab", ETabState::OpenedTab)
				->AddTab("ClassesExampleTab", ETabState::OpenedTab)
				->AddTab("FilesExampleTab", ETabState::OpenedTab)
			)
		);

	TSharedRef<SDockTab> TestSuite1Tab =
		SNew(SDockTab)
		.TabRole(ETabRole::MajorTab)
		.Label(LOCTEXT("FExamplePluginTabTitle", "ExamplePlugin"))
		.ToolTipText(LOCTEXT("FExamplePluginTabTip", "The tip for the example plugin main tab."));

	TestSuite1TabManager = FGlobalTabmanager::Get()->NewTabManager(TestSuite1Tab);

	TestSuite1TabManager->RegisterTabSpawner("AllExampleTab",
	                                         FOnSpawnTab::CreateStatic(&SpawnTab, FName("AllExampleTab")))
	                    .SetDisplayName(NSLOCTEXT("ExampleTabName", "AllExampleTab", "All"));

	TestSuite1TabManager->RegisterTabSpawner("ClassesExampleTab",
	                                         FOnSpawnTab::CreateStatic(&SpawnTab, FName("ClassesExampleTab")))
	                    .SetDisplayName(NSLOCTEXT("ExampleTabName", "ClassesExampleTab", "Classes"));

	TestSuite1TabManager->RegisterTabSpawner("FilesExampleTab",
	                                         FOnSpawnTab::CreateStatic(&SpawnTab, FName("FilesExampleTab")))
	                    .SetDisplayName(NSLOCTEXT("ExampleTabName", "FilesExampleTab", "Files"));

	TestSuite1Tab->SetContent
	(
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			TestSuite1TabManager->RestoreFrom(Layout, SpawnTabArgs.GetOwnerWindow()).ToSharedRef()
		]
	);
	// TestSuite1Tab.
	return TestSuite1Tab;
	/*return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];*/
}

TWeakPtr<SWindow> ExamplePluginWindow; // todo static or field?

void FExamplePluginModule::PluginButtonClicked()
{
	/*TSharedPtr<SWindow> ExistingWindow = MasterSequenceSettingsWindow.Pin();
	if (ExistingWindow.IsValid())
	{
		ExistingWindow->BringToFront();
	}

	TSharedRef<SWindow> TestWindow = SNew(SWindow)
					.ClientSize(FVector2D(640, 480))
					.Title(LOCTEXT("FExamplePluginTabTitle", "ExamplePlugin"))
					.AutoCenter(EAutoCenter::PrimaryWorkArea)
	[
		SNew(STextEditTest)
	];

	TestWindow->SetAllowFastUpdate(true); // todo what is it?

	FSlateApplication::Get().AddWindow(TestWindow);*/
	// FGlobalTabmanager::Get()->TryInvokeTab(ExamplePluginTabName);
	// UE_LOG(LogTemp, Log, TEXT("Natasha hi!") );

	TSharedPtr<SWindow> ExistingWindow = ExamplePluginWindow.Pin();
	if (ExistingWindow.IsValid())
	{
		ExistingWindow->BringToFront();
	}
	else
	{
		TSharedRef<SWindow> TestWindow = SNew(SWindow)
					.ClientSize(FVector2D(640, 480))
					.Title(LOCTEXT("FExamplePluginTabTitle", "ExamplePlugin"))
					.AutoCenter(EAutoCenter::PrimaryWorkArea)
		[
			SNew(STextEditTest)
		];
		TestWindow->SetAllowFastUpdate(true); // todo what is it?

		FSlateApplication::Get().AddWindow(TestWindow);
	}

	ExamplePluginWindow = ExistingWindow;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExamplePluginModule, ExamplePlugin)
