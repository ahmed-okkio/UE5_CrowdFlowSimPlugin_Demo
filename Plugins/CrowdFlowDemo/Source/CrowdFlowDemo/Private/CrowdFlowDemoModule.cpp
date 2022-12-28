// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowdFlowDemoModule.h"
#include "CrowdFlowDemoEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "CrowdFlowDemoModule"

void FCrowdFlowDemoModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FCrowdFlowDemoEditorModeCommands::Register();
}

void FCrowdFlowDemoModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FCrowdFlowDemoEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCrowdFlowDemoModule, CrowdFlowDemoEditorMode)