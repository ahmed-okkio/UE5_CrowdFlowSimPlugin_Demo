#include "Actors/CFAgent.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/CrowdFlowFinalDestination.h"
#include "CrowdFlowSimulationState.h"
#include "Actors/CrowdFlowExitSign.h"
#include "AIController.h"

ACFAgent::ACFAgent()
{
}

void ACFAgent::BeginPlay()
{
    Super::BeginPlay();
    TArray<AActor*> AllActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowFinalDestination::StaticClass(), AllActors);

    if (AllActors[0])
    {
        FinalDestination = AllActors[0]->GetActorLocation();

    }
	StartSimulating();
}

void ACFAgent::MoveToLocation(FVector& Loc)
{
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->MoveToLocation(Loc);
    }
}

void ACFAgent::StartSimulating()
{
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowFinalDestination::StaticClass(), AllActors);

	if (AllActors[0])
	{
		FinalDestination = AllActors[0]->GetActorLocation();
	}

	NearestExitLocation = GetNearestExitLocation();
	AgentData.StartingDistanceFromDest = FVector::Distance(GetActorLocation(), FinalDestination);

	if (ACrowdFlowSimulationState* SimState = Cast<ACrowdFlowSimulationState>(GetWorld()->GetGameState()))
	{
		AgentData.StartTime = SimState->GetTimeInHMS();
	}

    MoveToLocation(NearestExitLocation);
	//BeginLookingForDirectMoveToFinalDestination();
	//CalculateNextMove();
	//ExecuteNextMove();
}

FVector ACFAgent::GetNearestExitLocation()
{
	TArray<AActor*> Exits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowExitSign::StaticClass(), Exits);
	ACrowdFlowExitSign* NearestExit = nullptr;
	for (auto Exit : Exits)
	{
		ACrowdFlowExitSign* ExitSign = Cast<ACrowdFlowExitSign>(Exit);
		if (NearestExit == nullptr)
		{
			NearestExit = ExitSign;
		}
		else
		{
			if (!IsExitOnSameFloor(ExitSign->GetExitSignDestination()))
			{
				continue;
			}

			float DistanceToNewExit = FVector::Distance(GetActorLocation(), ExitSign->GetExitSignDestination());
			float DistanceToNearestExit = FVector::Distance(GetActorLocation(), NearestExit->GetExitSignDestination());

			if (DistanceToNewExit < DistanceToNearestExit)
			{
				NearestExit = ExitSign;
			}
		}
	}
	return NearestExit ? NearestExit->GetActorLocation() : FVector(0, 0, 0);
}

bool ACFAgent::IsExitOnSameFloor(FVector ExitLocation) const
{
	float CurrentHeight = GetActorLocation().Z;
	float LowerBound = ExitLocation.Z - SameFloorHeightMargin;
	float UpperBound = ExitLocation.Z + SameFloorHeightMargin;

	return (CurrentHeight >= LowerBound && CurrentHeight <= UpperBound);
}

FVector ACFAgent::GetFinalDestination() const
{
    return FinalDestination;
}
