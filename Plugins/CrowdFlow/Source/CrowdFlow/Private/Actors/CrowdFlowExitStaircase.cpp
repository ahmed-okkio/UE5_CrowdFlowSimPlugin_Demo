// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/KismetSystemLibrary.h"
#include "Components/BillboardComponent.h"
#include "Actors/CrowdFlowAgent.h"	
#include "Actors/CrowdFlowExitStaircase.h"

// Sets default values
ACrowdFlowExitStaircase::ACrowdFlowExitStaircase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard Icon"));
	SpriteComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ACrowdFlowExitStaircase::BeginPlay()
{
	Super::BeginPlay();
	BeginTraceForAgents();
	
	
}

bool ACrowdFlowExitStaircase::ShouldTickIfViewportsOnly() const
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

void ACrowdFlowExitStaircase::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ACrowdFlowExitStaircase, SeeBoundingBoxThroughWalls))
	{
		if (SeeBoundingBoxThroughWalls)
		{
			DepthPriority = -1;
			SpriteComponent->SetDepthPriorityGroup(ESceneDepthPriorityGroup::SDPG_MAX);
		}
		else
		{
			DepthPriority = 0;
			SpriteComponent->SetDepthPriorityGroup(ESceneDepthPriorityGroup::SDPG_World);
		}
	}
	Super::PostEditChangeProperty(e);
}

// Called every frame
void ACrowdFlowExitStaircase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasActorBegunPlay() && DrawDetectionDebug)
	{

		FVector Center = GetActorLocation();

		//Center.Z += BoundingBox.Z;

		DrawDebugBox(GetWorld(), Center, BoundingBox, GetActorRotation().Quaternion(), FColor::Green, false, 0.f, DepthPriority, 5);
	}

}

void ACrowdFlowExitStaircase::BeginTraceForAgents()
{
	GetWorld()->GetTimerManager().SetTimer(TH_AgentTrace, this, &ACrowdFlowExitStaircase::TraceForAgents, TraceRate, true);
}

void ACrowdFlowExitStaircase::TraceForAgents()
{
	FVector Center = GetActorLocation();
	FRotator Rotation = GetActorRotation();
	TArray<FHitResult> HitResults;
	TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectTypes;
	TArray<AActor*, FDefaultAllocator> ignoredActors;

	HitObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	bool bHit = UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetWorld(),
		Center,
		Center,
		BoundingBox,
		Rotation,
		HitObjectTypes,
		false,
		ignoredActors,
		EDrawDebugTrace::ForDuration,
		HitResults,
		true,
		FLinearColor::Blue,
		FLinearColor::Green,
		TraceRate
	);

	if (!bHit)
	{
		return;
	}

	for (auto HitResult : HitResults)
	{
		ACrowdFlowAgent* Agent = (ACrowdFlowAgent*)HitResult.GetActor();

		if (!Agent)
		{
			return;
		}

		Agent->MoveDownStair(this, RightSideGoesDown);

	}
}
		

