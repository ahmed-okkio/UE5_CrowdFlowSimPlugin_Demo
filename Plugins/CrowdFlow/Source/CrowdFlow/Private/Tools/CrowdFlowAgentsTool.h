// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"
#include "Actors/CrowdFlowAgent.h"
#include "CrowdFlowAgentsTool.generated.h"

/**
 * 
 */
UCLASS()
class UCrowdFlowAgentsToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};


/**
 * Property set for the UCrowdFlowInteractiveTool
 */
UCLASS(Transient)
class CROWDFLOW_API UCrowdFlowAgentsToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UCrowdFlowAgentsToolProperties();
	
	/*Behaviour of Agents to spawn*/
	UPROPERTY(EditAnywhere, Category = Options)
	EAgentBehaviour AgentBehaviour = EAgentBehaviour::FollowTheCrowd;


	///** Second point of measurement */
	//UPROPERTY(EditAnywhere, Category = Options)
	//FVector EndPoint;
	//
	///** Current distance measurement */
	//UPROPERTY(EditAnywhere, Category = Options)
	//double Distance;
};



/**
 * UCrowdFlowInteractiveTool is an example Tool that allows the user to measure the
 * distance between two points. The first point is set by click-dragging the mouse, and
 * the second point is set by shift-click-dragging the mouse.
 */
UCLASS()
class CROWDFLOW_API UCrowdFlowAgentsTool : public UInteractiveTool, public IClickBehaviorTarget
{
	GENERATED_BODY()

public:
	UCrowdFlowAgentsTool();

	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	virtual void Setup() override;
	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;


	/** IClickBehaviorTarget implementation */
	virtual FInputRayHit IsHitByClick(const FInputDeviceRay& ClickPos) override;

	virtual void OnClicked(const FInputDeviceRay& ClickPos) override;


	///** IClickDragBehaviorTarget implementation */
	//virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	//virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
	//virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
	//// these are not used in this Tool
	//virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override {}
	//virtual void OnTerminateDragSequence() override {}

	/** IModifierToggleBehaviorTarget implementation (inherited via IClickDragBehaviorTarget) */
	virtual void OnUpdateModifierState(int ModifierID, bool bIsOn) override;


protected:
	/** Properties of the tool are stored here */
	UPROPERTY()
		TObjectPtr<UCrowdFlowAgentsToolProperties> Properties;


protected:

	UClass* FTCClass = nullptr;
	UClass* ATCClass = nullptr;
	UClass* AUEClass = nullptr;

	UWorld* TargetWorld = nullptr;		// target World we will raycast into

	static const int MoveSecondPointModifierID = 1;		// identifier we associate with the shift key
	bool bSecondPointModifierDown = false;				// flag we use to keep track of modifier state
	bool bMoveSecondPoint = false;						// flag we use to keep track of which point we are moving during a press-drag

	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);		// raycasts into World
	void UpdatePosition(const FRay& WorldRay);					// updates first or second point based on raycast
	void UpdateDistance();										// updates distance
};
