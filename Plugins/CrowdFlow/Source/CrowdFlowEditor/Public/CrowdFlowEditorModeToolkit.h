// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"
#include "CrowdFlowEditorMode.h"

/**
 * This FModeToolkit just creates a basic UI panel that allows various InteractiveTools to
 * be initialized, and a DetailsView used to show properties of the active Tool.
 */
class FCrowdFlowEditorModeToolkit : public FModeToolkit
{
public:
	FCrowdFlowEditorModeToolkit();

	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode) override;
	virtual void GetToolPaletteNames(TArray<FName>& PaletteNames) const override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
#endif
};
