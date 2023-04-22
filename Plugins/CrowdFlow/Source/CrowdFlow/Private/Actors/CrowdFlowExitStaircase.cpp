// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/KismetSystemLibrary.h"
#include "Components/BillboardComponent.h"
#include "Actors/CrowdFlowAgent.h"	
#include "Actors/CrowdFlowExitStaircase.h"
#include "GameMode/CrowdFlowGameMode.h"

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
	ACrowdFlowGameMode* GM = Cast<ACrowdFlowGameMode>(GetWorld()->GetAuthGameMode());
	
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
#if WITH_EDITOR
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
#endif
// Called every frame
void ACrowdFlowExitStaircase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasActorBegunPlay() && DrawDetectionDebug)
	{

		FVector Center = SpriteComponent->GetComponentLocation();

		ACrowdFlowGameMode* GM = Cast<ACrowdFlowGameMode>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			if (GM->GetDebugMode())
			{
				DrawDebugBox(GetWorld(), Center, BoundingBox, GetActorRotation().Quaternion(), FColor::Green, false, 0.f, DepthPriority, 5);
			}
		}
	}

}

bool ACrowdFlowExitStaircase::IsRightSideStaircase() const
{
	return RightSideGoesDown;
}

void ACrowdFlowExitStaircase::BeginTraceForAgents()
{
	GetWorld()->GetTimerManager().SetTimer(TH_AgentTrace, this, &ACrowdFlowExitStaircase::TraceForAgents, TraceRate, true);
}

void ACrowdFlowExitStaircase::TraceForAgents()
{
	FVector Center = SpriteComponent->GetComponentLocation();
	FRotator Rotation = GetActorRotation();
	TArray<FHitResult> HitResults;
	TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectTypes;
	TArray<AActor*, FDefaultAllocator> ignoredActors;

	ACrowdFlowGameMode* GM = Cast<ACrowdFlowGameMode>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
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
			GM->GetDebugMode() ? FLinearColor::Blue : FLinearColor::Transparent,
			GM->GetDebugMode() ? FLinearColor::Green : FLinearColor::Transparent,
			TraceRate
		);
		if (!bHit)
		{
			return;
		}

	}
	
	for (auto HitResult : HitResults)
	{
		ACrowdFlowAgent* Agent = Cast<ACrowdFlowAgent>(HitResult.GetActor());

		if (!Agent)
		{
			return;
		}

		Agent->MoveDownStair(this);

	}
}
		

