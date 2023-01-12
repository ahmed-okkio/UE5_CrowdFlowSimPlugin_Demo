// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowdFlowEditorModeCommands.h"
#include "CrowdFlowEditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "CrowdFlowEditorModeCommands"

FCrowdFlowEditorModeCommands::FCrowdFlowEditorModeCommands()
	: TCommands<FCrowdFlowEditorModeCommands>("CrowdFlowEditorMode",
		NSLOCTEXT("CrowdFlowEditorMode", "CrowdFlowEditorModeCommands", "CrowdFlow Editor Mode"),
		NAME_None,
		FEditorStyle::GetStyleSetName())
{
}

void FCrowdFlowEditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FCrowdFlowEditorModeCommands::GetCommands()
{
	return FCrowdFlowEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
