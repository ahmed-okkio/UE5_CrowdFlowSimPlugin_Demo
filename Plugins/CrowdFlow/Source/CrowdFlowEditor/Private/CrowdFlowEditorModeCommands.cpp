// Copyright Epic Games, Inc. All Rights Reserved.
#if WITH_EDITOR

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

	UI_COMMAND(AgentsTool, "Agents", "Opens a menu to select and place agents of different behaviours in the 3D enviornment.", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(AgentsTool);

	UI_COMMAND(ExitsTool, "Exits", "Opens a menu to select place exit signs around the 3D enviornment.", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(ExitsTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FCrowdFlowEditorModeCommands::GetCommands()
{
	return FCrowdFlowEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
#endif