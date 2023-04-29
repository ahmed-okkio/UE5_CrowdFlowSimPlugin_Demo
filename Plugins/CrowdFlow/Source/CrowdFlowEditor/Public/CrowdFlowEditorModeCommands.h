// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * This class contains info about the full set of commands used in this editor mode.
 */
class FCrowdFlowEditorModeCommands : public TCommands<FCrowdFlowEditorModeCommands>
{
public:
	FCrowdFlowEditorModeCommands();
	virtual void RegisterCommands() override;
	static TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands();

	TSharedPtr<FUICommandInfo> AgentsTool;
	TSharedPtr<FUICommandInfo> ExitsTool;
	TSharedPtr<FUICommandInfo> StaircaseTool;


protected:
	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;
};
#endif
