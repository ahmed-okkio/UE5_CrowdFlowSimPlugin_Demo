// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowdFlowDemoEditorMode.h"
#include "CrowdFlowDemoEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "CrowdFlowDemoEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/CrowdFlowDemoSimpleTool.h"
#include "Tools/CrowdFlowDemoInteractiveTool.h"

// step 2: register a ToolBuilder in FCrowdFlowDemoEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "CrowdFlowDemoEditorMode"

const FEditorModeID UCrowdFlowDemoEditorMode::EM_CrowdFlowDemoEditorModeId = TEXT("EM_CrowdFlowDemoEditorMode");

FString UCrowdFlowDemoEditorMode::SimpleToolName = TEXT("CrowdFlowDemo_ActorInfoTool");
FString UCrowdFlowDemoEditorMode::InteractiveToolName = TEXT("CrowdFlowDemo_MeasureDistanceTool");


UCrowdFlowDemoEditorMode::UCrowdFlowDemoEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UCrowdFlowDemoEditorMode::EM_CrowdFlowDemoEditorModeId,
		LOCTEXT("ModeName", "CrowdFlowDemo"),
		FSlateIcon(),
		true);
}


UCrowdFlowDemoEditorMode::~UCrowdFlowDemoEditorMode()
{
}


void UCrowdFlowDemoEditorMode::ActorSelectionChangeNotify()
{
}

void UCrowdFlowDemoEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FCrowdFlowDemoEditorModeCommands& SampleToolCommands = FCrowdFlowDemoEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UCrowdFlowDemoSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UCrowdFlowDemoInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UCrowdFlowDemoEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FCrowdFlowDemoEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UCrowdFlowDemoEditorMode::GetModeCommands() const
{
	return FCrowdFlowDemoEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
