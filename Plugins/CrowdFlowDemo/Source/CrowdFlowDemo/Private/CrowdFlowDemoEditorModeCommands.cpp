// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowdFlowDemoEditorModeCommands.h"
#include "CrowdFlowDemoEditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "CrowdFlowDemoEditorModeCommands"

FCrowdFlowDemoEditorModeCommands::FCrowdFlowDemoEditorModeCommands()
	: TCommands<FCrowdFlowDemoEditorModeCommands>("CrowdFlowDemoEditorMode",
		NSLOCTEXT("CrowdFlowDemoEditorMode", "CrowdFlowDemoEditorModeCommands", "CrowdFlowDemo Editor Mode"),
		NAME_None,
		FEditorStyle::GetStyleSetName())
{
}

void FCrowdFlowDemoEditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FCrowdFlowDemoEditorModeCommands::GetCommands()
{
	return FCrowdFlowDemoEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
