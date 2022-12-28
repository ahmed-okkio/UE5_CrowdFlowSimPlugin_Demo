// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowdFlowDemoEditorModeToolkit.h"
#include "CrowdFlowDemoEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "CrowdFlowDemoEditorModeToolkit"

FCrowdFlowDemoEditorModeToolkit::FCrowdFlowDemoEditorModeToolkit()
{
}

void FCrowdFlowDemoEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FCrowdFlowDemoEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FCrowdFlowDemoEditorModeToolkit::GetToolkitFName() const
{
	return FName("CrowdFlowDemoEditorMode");
}

FText FCrowdFlowDemoEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "CrowdFlowDemoEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
