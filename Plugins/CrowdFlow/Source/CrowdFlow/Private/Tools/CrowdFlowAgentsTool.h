// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"
#include "CrowdFlowAgentsTool.generated.h"



UENUM(BlueprintType)
enum class EAgentBehaviour : uint8
{
	FollowTheCrowd UMETA(DisplayName = "Follow The Crowd"),
	AvoidTheCrowd UMETA(DisplayName = "Avoid The Crowd"),
	AvoidUnknownExits UMETA(DisplayName = "Avoid Unkown Exits")
};
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

class ACrowdFlowAgent;

/**
 * Property set for the UCrowdFlowInteractiveTool
 */
UCLASS(Transient)
class CROWDFLOW_API UCrowdFlowAgentsToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UCrowdFlowAgentsToolProperties();
	
	/*Toggle placing agents*/
	UPROPERTY(EditAnywhere, Category = Options)
	bool PlaceAgentOnClick = false;

	/*Behaviour of Agents to spawn*/
	UPROPERTY(EditAnywhere, Category = Options)
	EAgentBehaviour AgentBehaviour = EAgentBehaviour::FollowTheCrowd;

	/*List of agents in the world*/
	UPROPERTY(VisibleAnywhere, Category = Options)
	TArray<ACrowdFlowAgent*> AgentsInWorld;
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

	/** IClickBehaviorTarget implementation */
	virtual FInputRayHit IsHitByClick(const FInputDeviceRay& ClickPos) override;
	virtual void OnClicked(const FInputDeviceRay& ClickPos) override;

protected:
	/** Properties of the tool are stored here */
	UPROPERTY()
	TObjectPtr<UCrowdFlowAgentsToolProperties> Properties;

protected:
	UClass* ExitSignClass = nullptr;
	UClass* ExitStaircaseClass = nullptr;
	UClass* AUEClass = nullptr;

	UWorld* TargetWorld = nullptr;		// target World we will raycast into

	UFUNCTION()
	void OnActorRemovedFromWorld(AActor* DestroyedActor);
};
