// Copyright Epic Games, Inc. All Rights Reserved.
#if WITH_EDITOR

#include "CrowdFlowEditorModeToolkit.h"
#include "CrowdFlowEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "CrowdFlowEditorModeToolkit"
FCrowdFlowEditorModeToolkit::FCrowdFlowEditorModeToolkit()
{
}

void FCrowdFlowEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FCrowdFlowEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FCrowdFlowEditorModeToolkit::GetToolkitFName() const
{
	return FName("CrowdFlowEditorMode");
}

FText FCrowdFlowEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "CrowdFlowEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
#endif