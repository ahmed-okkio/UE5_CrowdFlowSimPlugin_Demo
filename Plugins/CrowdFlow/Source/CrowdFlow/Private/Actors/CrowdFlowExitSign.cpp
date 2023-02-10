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
	BoundingBox = FVector(100, 100, 100);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard Icon"));
	SpriteComponent->SetupAttachment(RootComponent);
	//SpriteComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

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
		FVector Center = (GetActorLocation() + Offset) + (GetActorForwardVector().GetSafeNormal() * BoundingBox.X);
		FRotator Rotation = GetActorRotation();
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

void ACrowdFlowExitSign::StopTraceForAgents()
{
	GetWorld()->GetTimerManager().ClearTimer(TH_AgentTrace);
}

// Called every frame
void ACrowdFlowExitSign::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!HasActorBegunPlay() && DrawDetectionDebug)
	{
		FVector Center = (GetActorLocation() + Offset) + GetActorForwardVector() * BoundingBox.X;
		DrawDebugBox(GetWorld(), Center, BoundingBox, GetActorRotation().Quaternion(), FColor::Green, false);
	}
}

FVector ACrowdFlowExitSign::GetExitSignDestination() const
{
	return ExitSignAgentDestination;
}

