// Fill out your copyright notice in the Description page of Project Settings.

#include "Tools/CrowdFlowAgentsTool.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/SingleClickBehavior.h"
#include "Actors/CrowdFlowAgent.h"
#include "Actors/CrowdFlowAgentFTC.h"
#include "Editor/EditorEngine.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "SceneManagement.h"
#include "CrowdFlowAgentsTool.h"

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

UCrowdFlowAgentsToolProperties::UCrowdFlowAgentsToolProperties()
{
}


UCrowdFlowAgentsTool::UCrowdFlowAgentsTool()
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> FTCAsset(TEXT("/CrowdFlow/BP_FTCAgent.BP_FTCAgent"));
	if (FTCAsset.Succeeded())
	{
			if (UBlueprint* Blueprint = FTCAsset.Object)
			{
				ExitSignClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
			}
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> ATCAsset(TEXT("/CrowdFlow/BP_AgentATC.BP_AgentATC"));
	if (ATCAsset.Succeeded())
	{
		if (UBlueprint* Blueprint = ATCAsset.Object)
		{
			ExitStaircaseClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
		}
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> AUEAsset(TEXT("/CrowdFlow/BP_AUEAgent.BP_AUEAgent"));
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
	MouseBehavior->Initialize(this);
	AddInputBehavior(MouseBehavior);

	// Create the property set and register it with the Tool
	Properties = NewObject<UCrowdFlowAgentsToolProperties>(this, "AgentsToolProperties");
	AddToolPropertySource(Properties);

	TArray<AActor*> Agents;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowAgent::StaticClass(), Agents);

	for (AActor* Actor : Agents)
	{
		ACrowdFlowAgent* Agent = Cast<ACrowdFlowAgent>(Actor);
		if (Agent)
		{
			Agent->OnDestroyed.AddUniqueDynamic(this, &UCrowdFlowAgentsTool::OnActorRemovedFromWorld);
			Properties->AgentsInWorld.AddUnique(Agent);
		}
	}
}

void UCrowdFlowAgentsTool::OnClicked(const FInputDeviceRay& ClickPos)
{
}

void UCrowdFlowAgentsTool::OnActorRemovedFromWorld(AActor* DestroyedActor)
{
	ACrowdFlowAgent* DestroyedAgent = Cast<ACrowdFlowAgent>(DestroyedActor);
	if (DestroyedAgent)
	{
		Properties->AgentsInWorld.Remove(DestroyedAgent);
	}
}

FInputRayHit UCrowdFlowAgentsTool::IsHitByClick(const FInputDeviceRay& ClickPos)
{
	if (!Properties->PlaceAgentOnClick)
	{
		return FInputRayHit();
	}

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

		AActor* SpawnedActor = nullptr;
		FVector Location = Hit.Location;
		Location.Z = Location.Z + (ACrowdFlowAgent::GetSphereRadius() / 2);
		switch (Properties->AgentBehaviour)
		{
			case EAgentBehaviour::FollowTheCrowd:
			{
				// Spawn actor with FollowTheCrowd behavior
				SpawnedActor = GetWorld()->SpawnActor(ExitSignClass, &Location, &FRotator::ZeroRotator, FActorSpawnParameters());
				break;
			}
			case EAgentBehaviour::AvoidTheCrowd:
			{
				// Spawn actor with AvoidTheCrowd behavior
				SpawnedActor = GetWorld()->SpawnActor(ExitStaircaseClass, &Location, &FRotator::ZeroRotator, FActorSpawnParameters());
				break;
			}
			case EAgentBehaviour::AvoidUnknownExits:
			{
				// Spawn actor with AvoidUnknownExits behavior
				SpawnedActor = GetWorld()->SpawnActor(AUEClass, &Location, &FRotator::ZeroRotator, FActorSpawnParameters());
				break;
			}
			default:
			{
				// Handle default case
				break;
			}
		}
		if (SpawnedActor)
		{
			SpawnedActor->OnDestroyed.AddUniqueDynamic(this, &UCrowdFlowAgentsTool::OnActorRemovedFromWorld);
			Properties->AgentsInWorld.AddUnique(Cast<ACrowdFlowAgent>(SpawnedActor));
		}

	}
	return FInputRayHit();
}
#undef LOCTEXT_NAMESPACE

