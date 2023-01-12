// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowdFlowModule.h"
#include "CrowdFlowEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "CrowdFlowModule"

void FCrowdFlowModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FCrowdFlowEditorModeCommands::Register();
}

void FCrowdFlowModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FCrowdFlowEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCrowdFlowModule, CrowdFlowEditorMode)