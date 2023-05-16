// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowExitSign.h"
//#include "Kismet/KismetSystemLibrary.h"
#include "Engine/TriggerVolume.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "CollisionDebugDrawingPublic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BillboardComponent.h"
#include "Actors/CrowdFlowAgent.h"
#include "Actors/CFAgent.h"
#include "GameMode/CrowdFlowGameMode.h"

// Sets default values
ACrowdFlowExitSign::ACrowdFlowExitSign()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DetectionRange = FVector(100, 100, 100);
	PhysicalExitBounds = FVector(50, 100, 100);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

// Called when the game starts or when spawned
void ACrowdFlowExitSign::BeginPlay()
{
	Super::BeginPlay();

	BeginTraceForAgents();
}

void ACrowdFlowExitSign::PostLoad()
{
	Super::PostLoad();
}

void ACrowdFlowExitSign::PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph)
{
	Super::PostLoadSubobjects(OuterInstanceGraph);

}

void ACrowdFlowExitSign::PostInitProperties()
{
	Super::PostInitProperties();

}

void ACrowdFlowExitSign::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();
}

bool ACrowdFlowExitSign::ShouldTickIfViewportsOnly() const
{
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ACrowdFlowExitSign::BeginTraceForAgents()
{
	GetWorld()->GetTimerManager().SetTimer(TH_AgentTrace, this, &ACrowdFlowExitSign::TraceForAgents, TraceRate, true);	
}

void ACrowdFlowExitSign::TraceForAgents()
{		
		ACrowdFlowGameMode* GM = Cast<ACrowdFlowGameMode>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			if (!GM->IsSimulationStarted())
			{
				return;
			}
		}
		FVector Center = (GetActorLocation() + DetectionRangeOffset) + (GetActorForwardVector().GetSafeNormal() * DetectionRange.X);
		FRotator Rotation = GetActorRotation();
		TArray<FHitResult> HitResults;
		TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectTypes;
		TArray<AActor*, FDefaultAllocator> ignoredActors;

		HitObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
		bool bHit = GetWorld()->SweepMultiByChannel(HitResults, Center, Center, Rotation.Quaternion(), ECollisionChannel::ECC_GameTraceChannel1, FCollisionShape::MakeBox(DetectionRange), FCollisionQueryParams());


		if (!bHit)
		{


			if (GM)
			{
				if (GM->GetDebugMode())
				{

					DrawDebugBox(GetWorld(), Center, DetectionRange, GetActorRotation().Quaternion(), KnownExit ? FColor::Emerald : FColor::Orange, false, TraceRate);
				}
			}
			else
			{
				DrawDebugBox(GetWorld(), Center, DetectionRange, GetActorRotation().Quaternion(), KnownExit ? FColor::Emerald : FColor::Orange, false);
			}
			
			return;
		}

		for (auto HitResult : HitResults)
		{
			//ACrowdFlowAgent* Agent = Cast<ACrowdFlowAgent>(HitResult.GetActor());
			ACFAgent* Agent = Cast<ACFAgent>(HitResult.GetActor());

			if (!Agent)

			{
				return;
			}



			if (GM)
			{
				if (GM->GetDebugMode())
				{
					DrawDebugBox(GetWorld(), Center, DetectionRange, GetActorRotation().Quaternion(), KnownExit ? FColor::Emerald : FColor::Orange, false, TraceRate, 0, 2);
				}
			}
			else
			{
				DrawDebugBox(GetWorld(), Center, DetectionRange, GetActorRotation().Quaternion(), KnownExit ? FColor::Emerald : FColor::Orange, false);
			}

			if (!Agents.Contains(Agent))
			{
				Agent->SeeExitSign(this);
			}
		}
}

void ACrowdFlowExitSign::StopTraceForAgents()
{
	GetWorld()->GetTimerManager().ClearTimer(TH_AgentTrace);
}

// Called every frame
void ACrowdFlowExitSign::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!HasActorBegunPlay() && ShowDetectionRange)
	{
		FVector Center = (GetActorLocation() + DetectionRangeOffset) + GetActorForwardVector() * DetectionRange.X;
		ACrowdFlowGameMode* GM = Cast<ACrowdFlowGameMode>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			if (GM->GetDebugMode())
			{
				DrawDebugBox(GetWorld(), Center, DetectionRange, GetActorRotation().Quaternion(), KnownExit ? FColor::Emerald : FColor::Orange, false);
			}
		}
		else
		{
			DrawDebugBox(GetWorld(), Center, DetectionRange, GetActorRotation().Quaternion(), KnownExit ? FColor::Emerald : FColor::Orange, false);
		}
	}

	if (!HasActorBegunPlay() && ShowPhysicalExitBounds)
	{
		FVector Center = (GetActorLocation() + PhysicalExitOffset) + GetActorForwardVector() * PhysicalExitBounds.X;
		ACrowdFlowGameMode* GM = Cast<ACrowdFlowGameMode>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			if (GM->GetDebugMode())
			{
				DrawDebugBox(GetWorld(), Center, PhysicalExitBounds, GetActorRotation().Quaternion(), FColor::Silver, false);
			}
		}
		else
		{
			DrawDebugBox(GetWorld(), Center, PhysicalExitBounds, GetActorRotation().Quaternion(), FColor::Silver, false);
		}
	}
}

FVector ACrowdFlowExitSign::GetExitSignDestination() const
{
	return ((GetActorLocation() + PhysicalExitOffset) + GetActorForwardVector() * PhysicalExitBounds.X) + (-GetActorForwardVector() * DistancePastSign);
}

bool ACrowdFlowExitSign::IsKnownExit() const
{
	return KnownExit;
}

int32 ACrowdFlowExitSign::GetAgentCount(ACFAgent* CheckingAgent) const
{
	if (Agents.Contains(CheckingAgent))
	{
		return Agents.Num() - 1;

	}

	return Agents.Num();

}

void ACrowdFlowExitSign::FollowSign(ACFAgent* FollowingAgent)
{
	Agents.AddUnique(FollowingAgent);
}

void ACrowdFlowExitSign::UnfollowSign(ACFAgent* FollowingAgent)
{
	Agents.Remove(FollowingAgent);
}


