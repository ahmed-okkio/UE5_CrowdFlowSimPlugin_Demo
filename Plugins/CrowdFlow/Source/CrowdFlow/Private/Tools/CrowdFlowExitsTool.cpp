// Fill out your copyright notice in the Description page of Project Settings.

#include "Tools/CrowdFlowExitsTool.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/SingleClickBehavior.h"
#include "Actors/CrowdFlowExitSign.h"
#include "Actors/CrowdFlowExitStaircase.h"
#include "Actors/CrowdFlowFinalDestination.h"
#include "Actors/CrowdFlowAgentFTC.h"
#include "Editor/EditorEngine.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "SceneManagement.h"

// localization namespace
#define LOCTEXT_NAMESPACE "UCrowdFlowExitsTool"

/*
 * ToolBuilder
 */

UInteractiveTool* UCrowdFlowExitsToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UCrowdFlowExitsTool* NewTool = NewObject<UCrowdFlowExitsTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

UCrowdFlowExitsToolProperties::UCrowdFlowExitsToolProperties()
{
}

UCrowdFlowExitsTool::UCrowdFlowExitsTool()
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> ExitSignAsset(TEXT("/CrowdFlow/BP_ExitSign.BP_ExitSign"));
	if (ExitSignAsset.Succeeded())
	{
			if (UBlueprint* Blueprint = ExitSignAsset.Object)
			{
				ExitSignClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
			}
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> ExitStaircaseAsset(TEXT("/CrowdFlow/BP_ExitStaircase.BP_ExitStaircase"));
	if (ExitStaircaseAsset.Succeeded())
	{
		if (UBlueprint* Blueprint = ExitStaircaseAsset.Object)
		{
			ExitStaircaseClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
		}
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> FinalDestinationAsset(TEXT("/CrowdFlow/BP_FinalDestination.BP_FinalDestination"));
	if (FinalDestinationAsset.Succeeded())
	{
		if (UBlueprint* Blueprint = FinalDestinationAsset.Object)
		{
			FinalDestinationClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
		}
	}
}

void UCrowdFlowExitsTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}


void UCrowdFlowExitsTool::Setup()
{
	UInteractiveTool::Setup();
	// Add default mouse input behavior
	USingleClickInputBehavior* MouseBehavior = NewObject<USingleClickInputBehavior>();
	MouseBehavior->Initialize(this);
	AddInputBehavior(MouseBehavior);

	// Create the property set and register it with the Tool
	Properties = NewObject<UCrowdFlowExitsToolProperties>(this, "AgentsToolProperties");
	AddToolPropertySource(Properties);

	TArray<AActor*> ExitSigns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowExitSign::StaticClass(), ExitSigns);

	for (AActor* Actor : ExitSigns)
	{
		ACrowdFlowExitSign* ExitSign = Cast<ACrowdFlowExitSign>(Actor);
		if (ExitSign)
		{
			ExitSign->OnDestroyed.AddUniqueDynamic(this, &UCrowdFlowExitsTool::OnExitRemovedFromWorld);
			Properties->ExitSignsInTheWorld.AddUnique(ExitSign);
		}
	}

	TArray<AActor*> ExitStaircases;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowExitStaircase::StaticClass(), ExitStaircases);

	for (AActor* Actor : ExitStaircases)
	{
		ACrowdFlowExitStaircase* Staircase = Cast<ACrowdFlowExitStaircase>(Actor);
		if (Staircase)
		{
			Staircase->OnDestroyed.AddUniqueDynamic(this, &UCrowdFlowExitsTool::OnStaircaseRemovedFromWorld);
			Properties->ExitStaircasesInTheWorld.AddUnique(Staircase);
		}
	}

	TArray<AActor*> FinalDestinations;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowFinalDestination::StaticClass(), FinalDestinations);

	ACrowdFlowFinalDestination* FinalDestination = Cast<ACrowdFlowFinalDestination>(FinalDestinations[0]);

	if (FinalDestination)
	{
		FinalDestination->OnDestroyed.AddUniqueDynamic(this, &UCrowdFlowExitsTool::OnFinalDestinationRemovedFromWorld);
		Properties->FinalDestination = FinalDestination;
	}
	
}

FInputRayHit UCrowdFlowExitsTool::IsHitByClick(const FInputDeviceRay& ClickPos)
{
	if (!Properties->PlaceExitOnClick)
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
		switch (Properties->ToolMode)
		{
			case EExitToolMode::ExitSign:
			{
				// Spawn actor with FollowTheCrowd behavior
				SpawnedActor = GetWorld()->SpawnActor(ExitSignClass, &Location, &FRotator::ZeroRotator, FActorSpawnParameters());
				if (SpawnedActor)
				{
					SpawnedActor->OnDestroyed.AddUniqueDynamic(this, &UCrowdFlowExitsTool::OnExitRemovedFromWorld);
					Properties->ExitSignsInTheWorld.AddUnique(Cast<ACrowdFlowExitSign>(SpawnedActor));
				}
				break;
			}
			case EExitToolMode::ExitStaircase:
			{
				// Spawn actor with AvoidTheCrowd behavior
				SpawnedActor = GetWorld()->SpawnActor(ExitStaircaseClass, &Location, &FRotator::ZeroRotator, FActorSpawnParameters());
				if (SpawnedActor)
				{
					SpawnedActor->OnDestroyed.AddUniqueDynamic(this, &UCrowdFlowExitsTool::OnStaircaseRemovedFromWorld);
					Properties->ExitStaircasesInTheWorld.AddUnique(Cast<ACrowdFlowExitStaircase>(SpawnedActor));
				}
				break;
			}
			case EExitToolMode::FinalDestination:
			{
				// Spawn actor with AvoidTheCrowd behavior
				SpawnedActor = GetWorld()->SpawnActor(FinalDestinationClass, &Location, &FRotator::ZeroRotator, FActorSpawnParameters());
				if (SpawnedActor)
				{
					if (Properties->FinalDestination)
					{
						Properties->FinalDestination->Destroy();
					}
					SpawnedActor->OnDestroyed.AddUniqueDynamic(this, &UCrowdFlowExitsTool::OnFinalDestinationRemovedFromWorld);
					Properties->FinalDestination = Cast<ACrowdFlowFinalDestination>(SpawnedActor);
				}
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
			GEditor->SelectActor(SpawnedActor,true,true);
		}

	}
	return FInputRayHit();
}

void UCrowdFlowExitsTool::OnClicked(const FInputDeviceRay& ClickPos)
{

}
void UCrowdFlowExitsTool::OnExitRemovedFromWorld(AActor* DestroyedActor)
{
	ACrowdFlowExitSign* DestroyedAgent = Cast<ACrowdFlowExitSign>(DestroyedActor);
	if (DestroyedAgent)
	{
		Properties->ExitSignsInTheWorld.Remove(DestroyedAgent);
	}
}
void UCrowdFlowExitsTool::OnStaircaseRemovedFromWorld(AActor* DestroyedActor)
{
	ACrowdFlowExitStaircase* DestroyedAgent = Cast<ACrowdFlowExitStaircase>(DestroyedActor);
	if (DestroyedAgent)
	{
		Properties->ExitStaircasesInTheWorld.Remove(DestroyedAgent);
	}
}
void UCrowdFlowExitsTool::OnFinalDestinationRemovedFromWorld(AActor* DestroyedActor)
{
	//ACrowdFlowFinalDestination* DestroyedAgent = Cast<ACrowdFlowFinalDestination>(DestroyedActor);
	//if (DestroyedAgent)
	//{
	//	Properties->FinalDestination = nullptr;
	//}
}
#undef LOCTEXT_NAMESPACE
