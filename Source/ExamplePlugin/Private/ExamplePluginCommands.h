// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "UI/ExamplePluginStyle.h"

class FExamplePluginCommands : public TCommands<FExamplePluginCommands>
{
public:
	FExamplePluginCommands()
		: TCommands<FExamplePluginCommands>(
			TEXT("ExamplePlugin"), NSLOCTEXT("Contexts", "ExamplePlugin", "ExamplePlugin Plugin"), NAME_None,
			FExamplePluginStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
