// Fill out your copyright notice in the Description page of Project Settings.

#include "Tools/CrowdFlowAgentsTool.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/SingleClickBehavior.h"
#include "Actors/CrowdFlowAgent.h"
#include "Actors/CrowdFlowAgentFTC.h"
#include "Editor/EditorEngine.h"
#include "UObject/ConstructorHelpers.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "SceneManagement.h"

// localization namespace
#define LOCTEXT_NAMESPACE "UCrowdFlowAgentsTool"

/*
 * ToolBuilder
 */

UInteractiveTool* UCrowdFlowAgentsToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UCrowdFlowAgentsTool* NewTool = NewObject<UCrowdFlowAgentsTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}


// JAH TODO: update comments
/*
 * Tool
 */

UCrowdFlowAgentsToolProperties::UCrowdFlowAgentsToolProperties()
{
	// initialize the points and distance to reasonable values
}


UCrowdFlowAgentsTool::UCrowdFlowAgentsTool()
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> FTCAsset(TEXT("/CrowdFlow/BP_FTCAgent.BP_FTCAgent"));
	if (FTCAsset.Succeeded())
	{
			if (UBlueprint* Blueprint = FTCAsset.Object)
			{
				FTCClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
			}
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> ATCAsset(TEXT("/CrowdFlow/BP_AgentATC.BP_AgentATC"));
	if (ATCAsset.Succeeded())
	{
		if (UBlueprint* Blueprint = ATCAsset.Object)
		{
			ATCClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
		}
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> AUEAsset(TEXT("/CrowdFlow/BP_AgentAUE.BP_AgentAUE"));
	if (AUEAsset.Succeeded())
	{
		if (UBlueprint* Blueprint = AUEAsset.Object)
		{
			AUEClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
		}
	}
}

void UCrowdFlowAgentsTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}


void UCrowdFlowAgentsTool::Setup()
{
	UInteractiveTool::Setup();
	
	
	

	// Add default mouse input behavior
	USingleClickInputBehavior* MouseBehavior = NewObject<USingleClickInputBehavior>();
	// We will use the shift key to indicate that we should move the second point. 
	// This call tells the Behavior to call our OnUpdateModifierState() function on mouse-down and mouse-move
	//MouseBehavior->Modifiers.RegisterModifier(MoveSecondPointModifierID, FInputDeviceState::IsShiftKeyDown);
	MouseBehavior->Initialize(this);
	AddInputBehavior(MouseBehavior);

	// Create the property set and register it with the Tool
	Properties = NewObject<UCrowdFlowAgentsToolProperties>(this, "AgentsToolProperties");
	AddToolPropertySource(Properties);

	bSecondPointModifierDown = false;
	bMoveSecondPoint = false;
}


void UCrowdFlowAgentsTool::OnUpdateModifierState(int ModifierID, bool bIsOn)
{
	// keep track of the "second point" modifier (shift key for mouse input)
	if (ModifierID == MoveSecondPointModifierID)
	{
		bSecondPointModifierDown = bIsOn;
	}
}


//FInputRayHit UCrowdFlowAgentsTool::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
//{
//	// we only start drag if press-down is on top of something we can raycast
//	FVector Temp;
//	FInputRayHit Result = FindRayHit(PressPos.WorldRay, Temp);
//	return Result;
//}
//
//
//void UCrowdFlowAgentsTool::OnClickPress(const FInputDeviceRay& PressPos)
//{
//	// determine whether we are moving first or second point for the drag sequence
//	bMoveSecondPoint = bSecondPointModifierDown;
//	UpdatePosition(PressPos.WorldRay);
//}
//
//
//void UCrowdFlowAgentsTool::OnClickDrag(const FInputDeviceRay& DragPos)
//{
//	UpdatePosition(DragPos.WorldRay);
//}


FInputRayHit UCrowdFlowAgentsTool::FindRayHit(const FRay& WorldRay, FVector& HitPos)
{
	// trace a ray into the World
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;
	bool bHitWorld = TargetWorld->LineTraceSingleByObjectType(Result, WorldRay.Origin, WorldRay.PointAt(999999), QueryParams);
	if (bHitWorld)
	{
		HitPos = Result.ImpactPoint;
		return FInputRayHit(Result.Distance);
	}
	return FInputRayHit();
}


void UCrowdFlowAgentsTool::UpdatePosition(const FRay& WorldRay)
{
	//FInputRayHit HitResult = FindRayHit(WorldRay, (bMoveSecondPoint) ? Properties->EndPoint : Properties->StartPoint);
	//if (HitResult.bHit)
	//{
	//	UpdateDistance();
	//}
}


void UCrowdFlowAgentsTool::UpdateDistance()
{
	//Properties->Distance = FVector::Distance(Properties->StartPoint, Properties->EndPoint);
}


void UCrowdFlowAgentsTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	// if the user updated any of the property fields, update the distance
	UpdateDistance();
}

FInputRayHit UCrowdFlowAgentsTool::IsHitByClick(const FInputDeviceRay& ClickPos)
{
	FHitResult Hit;
	float Range = 10000.f;
	FVector Origin = ClickPos.WorldRay.Origin;
	FVector Endpoint = Origin + (ClickPos.WorldRay.Direction * Range);
	if (TargetWorld->LineTraceSingleByChannel(Hit, Origin, Endpoint, ECollisionChannel::ECC_WorldStatic))
	{
		UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
		if (!EditorEngine)
		{
			return FInputRayHit();

		}

		//ACrowdFlowAgentFTC* FTCAgent = NewObject<ACrowdFlowAgentFTC>();
		//FTCAgent->SetActorLocation(Hit.Location);

		TargetWorld->SpawnActor(FTCClass, &Hit.Location, &FRotator::ZeroRotator, FActorSpawnParameters());

		//EditorEngine->AddActor()

	}
	DrawDebugLine(GetWorld(), Origin, Endpoint, FColor::Red, true);

	return FInputRayHit();
}

void UCrowdFlowAgentsTool::OnClicked(const FInputDeviceRay& ClickPos)
{
}


void UCrowdFlowAgentsTool::Render(IToolsContextRenderAPI* RenderAPI)
{
	//FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();
	//// draw a thin line that shows through objects
	//PDI->DrawLine(Properties->StartPoint, Properties->EndPoint,
	//	FColor(240, 16, 16), SDPG_Foreground, 2.0f, 0.0f, true);
	//// draw a thicker line that is depth-tested
	//PDI->DrawLine(Properties->StartPoint, Properties->EndPoint,
	//	FColor(240, 16, 16), SDPG_World, 4.0f, 0.0f, true);
}


#undef LOCTEXT_NAMESPACE
