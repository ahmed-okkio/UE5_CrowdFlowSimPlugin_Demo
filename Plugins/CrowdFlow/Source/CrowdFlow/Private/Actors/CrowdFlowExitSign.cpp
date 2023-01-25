// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowExitSign.h"
//#include "Kismet/KismetSystemLibrary.h"
#include "Engine/TriggerVolume.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "CollisionDebugDrawingPublic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Actors/CrowdFlowAgent.h"

// Sets default values
ACrowdFlowExitSign::ACrowdFlowExitSign()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoundingBox = FVector(1, 1, 1);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

// Called when the game starts or when spawned
void ACrowdFlowExitSign::BeginPlay()
{
	Super::BeginPlay();
	ForwardArrow = FindComponentByClass<UArrowComponent>();

	ExitSignAgentDestination = GetActorLocation() + (ForwardArrow->GetForwardVector() * 50);
	BeginTraceForAgents();

}

void ACrowdFlowExitSign::BeginTraceForAgents()
{
	GetWorld()->GetTimerManager().SetTimer(TH_AgentTrace, this, &ACrowdFlowExitSign::TraceForAgents, TraceRate, true);	
}

void ACrowdFlowExitSign::TraceForAgents()
{
	if (ForwardArrow)
	{
		FVector Center = GetActorLocation() + (ForwardArrow->GetForwardVector() * BoundingBox.X);
		FRotator Rotation = ForwardArrow->GetForwardVector().Rotation();
		FHitResult HitResult;
		TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectTypes;
		TArray<AActor*, FDefaultAllocator> ignoredActors;

		HitObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
		bool bHit = UKismetSystemLibrary::BoxTraceSingleForObjects(
			GetWorld(),
			Center,
			Center,
			BoundingBox,
			Rotation,
			HitObjectTypes,
			false,
			ignoredActors,
			EDrawDebugTrace::ForDuration,
			HitResult,
			true,
			FLinearColor::Blue,
			FLinearColor::Green,
			TraceRate
		);

		if (bHit)
		{
			ACrowdFlowAgent* Agent = (ACrowdFlowAgent*) HitResult.GetActor();
			if (!Agent)
			{
				return;
			}
	
			Agent->MoveToExitSign(this);
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

}

FVector ACrowdFlowExitSign::GetExitSignDestination() const
{
	return ExitSignAgentDestination;
}

