// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#if WITH_EDITOR
#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseBehaviors/SingleClickBehavior.h"
#include "CrowdFlowExitsTool.generated.h"

/**
 * 
 */
class ACrowdFlowExitSign;
class ACrowdFlowExitStaircase;
class ACrowdFlowFinalDestination;
UENUM(BlueprintType)
enum class EExitToolMode : uint8
{
	ExitSign UMETA(DisplayName = "Exit Sign"),
	ExitStaircase UMETA(DisplayName = "Exit Staircase"),
	FinalDestination UMETA(DisplayName = "Final Destination")

};

UCLASS()
class UCrowdFlowExitsToolBuilder : public UInteractiveToolBuilder
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
class CROWDFLOWEDITOR_API UCrowdFlowExitsToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UCrowdFlowExitsToolProperties();
#endif

#if	WITH_EDITORONLY_DATA
	/*Toggle placing agents*/
	UPROPERTY(EditAnywhere, Category = Options)
	bool PlaceExitOnClick = false;

	/*Behaviour of Agents to spawn*/
	UPROPERTY(EditAnywhere, Category = Options)
	EExitToolMode ToolMode = EExitToolMode::ExitSign;

	/*List of agents in the world*/
	UPROPERTY(VisibleAnywhere, Category = Options)
	TArray<ACrowdFlowExitSign*> ExitSignsInTheWorld;

	UPROPERTY(VisibleAnywhere, Category = Options)
	TArray<ACrowdFlowExitStaircase*> ExitStaircasesInTheWorld;

	UPROPERTY(VisibleAnywhere, Category = Options)
	ACrowdFlowFinalDestination* FinalDestination;
#endif
#if WITH_EDITOR

};



/**
 * UCrowdFlowInteractiveTool is an example Tool that allows the user to measure the
 * distance between two points. The first point is set by click-dragging the mouse, and
 * the second point is set by shift-click-dragging the mouse.
 */
UCLASS()
class CROWDFLOWEDITOR_API UCrowdFlowExitsTool : public UInteractiveTool, public IClickBehaviorTarget
{
	GENERATED_BODY()

public:
	UCrowdFlowExitsTool();

	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	virtual void Setup() override;

	/** IClickBehaviorTarget implementation */
	virtual FInputRayHit IsHitByClick(const FInputDeviceRay& ClickPos) override;
	virtual void OnClicked(const FInputDeviceRay& ClickPos) override;

protected:
#endif

#if	WITH_EDITORONLY_DATA

	/** Properties of the tool are stored here */
	UPROPERTY()
		TObjectPtr<UCrowdFlowExitsToolProperties> Properties;
#endif
#if WITH_EDITOR


protected:

	UClass* ExitSignClass = nullptr;
	UClass* ExitStaircaseClass = nullptr;
	UClass* FinalDestinationClass = nullptr;

	UWorld* TargetWorld = nullptr;		// target World we will raycast into

	UFUNCTION()
	void OnExitRemovedFromWorld(AActor* DestroyedActor);
	UFUNCTION()
	void OnStaircaseRemovedFromWorld(AActor* DestroyedActor);
	UFUNCTION()
	void OnFinalDestinationRemovedFromWorld(AActor* DestroyedActor);
};
#endif