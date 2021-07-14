// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExamplePluginCommands.h"

#define LOCTEXT_NAMESPACE "FExamplePluginModule"

void FExamplePluginCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "ExamplePlugin", "Bring up ExamplePlugin window",  EUserInterfaceActionType::Button,  FInputChord(EModifierKey::Control, EKeys::T));
}

#undef LOCTEXT_NAMESPACE
