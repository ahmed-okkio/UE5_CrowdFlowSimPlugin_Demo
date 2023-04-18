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

	ExitSignAgentDestination = GetActorLocation() + (GetActorForwardVector() * AgentDestinationDistanceFromSign);
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

void ACrowdFlowExitSign::PostEditMove(bool bFinished)
{
	
	Super::PostEditMove(bFinished);
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
		FVector Center = (GetActorLocation() + DetectionRangeOffset) + (GetActorForwardVector().GetSafeNormal() * DetectionRange.X);
		FRotator Rotation = GetActorRotation();
		TArray<FHitResult> HitResults;
		TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectTypes;
		TArray<AActor*, FDefaultAllocator> ignoredActors;

		HitObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
		bool bHit = GetWorld()->SweepMultiByChannel(HitResults, Center, Center, Rotation.Quaternion(), ECollisionChannel::ECC_GameTraceChannel1, FCollisionShape::MakeBox(DetectionRange), FCollisionQueryParams());

		if (!bHit)
		{
			DrawDebugBox(GetWorld(), Center, DetectionRange, GetActorRotation().Quaternion(), KnownExit ? FColor::Emerald : FColor::Orange, false, TraceRate);
			return;
		}

		for (auto HitResult : HitResults)
		{
			ACrowdFlowAgent* Agent = Cast<ACrowdFlowAgent>(HitResult.GetActor());
			if (!Agent)

			{
				return;
			}

			DrawDebugBox(GetWorld(), Center, DetectionRange, GetActorRotation().Quaternion(), KnownExit ? FColor::Emerald : FColor::Orange, false, TraceRate, 0 , 2);
			FString Name = GetActorNameOrLabel();
			if(Name == "BP_ExitSign5")
			{
				FName t = FName();
				Name = "";
				Agent->SeeExitSign(this);

			}
			else
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
		DrawDebugBox(GetWorld(), Center, DetectionRange, GetActorRotation().Quaternion(), KnownExit? FColor::Emerald : FColor::Orange, false);
	}

	if (!HasActorBegunPlay() && ShowPhysicalExitBounds)
	{
		FVector Center = (GetActorLocation() + PhysicalExitOffset) + GetActorForwardVector() * PhysicalExitBounds.X;
		DrawDebugBox(GetWorld(), Center, PhysicalExitBounds, GetActorRotation().Quaternion(), FColor::Silver , false);
	}
}

FVector ACrowdFlowExitSign::GetExitSignDestination() const
{
	return (GetActorLocation() + PhysicalExitOffset) + GetActorForwardVector() * PhysicalExitBounds.X;
}

bool ACrowdFlowExitSign::IsKnownExit() const
{
	return KnownExit;
}

int32 ACrowdFlowExitSign::GetAgentCount() const
{
	return Agents.Num();
}

void ACrowdFlowExitSign::FollowSign(ACrowdFlowAgent* FollowingAgent) 
{
	Agents.AddUnique(FollowingAgent);
}

void ACrowdFlowExitSign::UnfollowSign(ACrowdFlowAgent* FollowingAgent)
{
	Agents.Remove(FollowingAgent);
}


