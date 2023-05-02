#include "Actors/CFAgent.h"

#include "Actors/CrowdFlowExitSign.h"
#include "Actors/CrowdFlowExitStaircase.h"
#include "Actors/CrowdFlowFinalDestination.h"
#include "AI/Navigation/NavQueryFilter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "CrowdFlowSimulationState.h"
#include "Kismet/GameplayStatics.h"

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

	SimState = Cast<ACrowdFlowSimulationState>(GetWorld()->GetGameState());
	

	StartSimulating();
}

void ACFAgent::MoveToLocation(FVector Loc)
{
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->MoveToLocation(Loc,105);
    }
}

void ACFAgent::StopMovement()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->StopMovement();
	}
}

void ACFAgent::RegisterSpeedAtTime()
{
	if(SimState)
	{
		float CurrentSpeed = GetMovementComponent()->Velocity.Size();
		AgentData.SpeedTimeData.Add(SimState->GetTimeInSeconds(), CurrentSpeed);
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
	AgentData.AgentName = GetActorNameOrLabel();
	AgentData.StartingDistanceFromDest = FVector::Distance(GetActorLocation(), FinalDestination);

	if (SimState)
	{
		AgentData.StartTime = SimState->GetTimeInHMS();
	}

	GetWorld()->GetTimerManager().SetTimer(TH_TrackTime, this, &ACFAgent::RegisterSpeedAtTime, 1.f, true, 0);

    MoveToLocation(FinalDestination);
}

void ACFAgent::EndSimulation()
{
	if (SimState)
	{
		GetWorld()->GetTimerManager().ClearTimer(TH_TrackTime);

		float TotalTimeSeconds = 0.0f;
		float TotalDistance = 0.0f;

		// Iterate through the map and calculate the distance traveled at each speed
		for (auto& Pair : AgentData.SpeedTimeData)
		{
			float SpeedCmPerSecond = Pair.Value;
			float TimeSeconds = Pair.Key;

			TotalTimeSeconds += TimeSeconds;

			// Calculate the distance traveled at this speed
			float DistanceAtSpeed = SpeedCmPerSecond * TimeSeconds;
			TotalDistance += DistanceAtSpeed;
		}

		// Calculate the average speed by dividing total distance by total time taken
		float AverageSpeedCmPerSecond = TotalDistance / TotalTimeSeconds;

		// Convert the average speed to cm/s
		float AverageSpeedCmPerS = AverageSpeedCmPerSecond / 100.0f;
		AgentData.AverageSpeed = AverageSpeedCmPerS;

		AgentData.EndTime = SimState->GetTimeInHMS();
		AgentData.Duration = AgentData.GetEvacuationDuration();
		SimState->SubmitAgentData(AgentData);
		Destroy();
	}
}

void ACFAgent::BeginLookingForDirectMoveToFinalDestination()
{
	GetWorld()->GetTimerManager().SetTimer(TH_DirectMove, this, &ACFAgent::AttemptDirectMoveToFinalDestination, DirectMoveSearchRate, true);
}

void ACFAgent::AttemptDirectMoveToFinalDestination()
{
	if (IsFinalDestinationVisible() && IsExitOnSameFloor(FinalDestination))
	{
		MoveToLocation(FinalDestination);

		FoundDirectMoveToExit = true;
	}
	else
	{
		FoundDirectMoveToExit = false;
	}
}

bool ACFAgent::IsFinalDestinationVisible()
{
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), FinalDestination, ECollisionChannel::ECC_GameTraceChannel2);
	return !Hit.bBlockingHit;
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

			float DistanceToNewExit = FVector::Distance(GetActorLocation(), ExitSign->GetActorLocation());
			float DistanceToNearestExit = FVector::Distance(GetActorLocation(), NearestExit->GetActorLocation());

			if (DistanceToNewExit < DistanceToNearestExit)
			{
				NearestExit = ExitSign;
			}
		}
	}
	return NearestExit ? NearestExit->GetExitSignDestination() : FVector(0, 0, 0);
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

void ACFAgent::SeeExitSign(ACrowdFlowExitSign* ExitSign)
{
	return;
	if (!ExitSign)
	{
		return;
	}

	if (ExitSign == ExitSignBeingFollowed || ExitSign == LastFollowedExitSign)
	{
		return;
	}

	if (ExitSignBeingFollowed)
	{
		// checking if the old exit sign is already behind us to see if we should consider new signs or not
		if (!IsExitSignBehind(ExitSignBeingFollowed))
		{
			return;
		}
	}

	ExitSignBeingFollowed = ExitSign;

	if (ExitSignBeingFollowed)
	{
		MoveToLocation(ExitSignBeingFollowed->GetExitSignDestination());
	}
}

bool ACFAgent::IsExitSignBehind(ACrowdFlowExitSign* ExitSign) const
{
	if (!ExitSign)
	{
		return false;
	}

	// Get the forward direction of the agent and normalize it
	FVector AgentForward = GetActorForwardVector().GetSafeNormal2D();

	FVector ExitLoc = ExitSign->GetActorLocation() + (-ExitSign->GetActorForwardVector() * 125);
	// Calculate the vector from the agent to the exit sign
	FVector AgentToExitSign = ExitLoc - GetActorLocation();
	AgentToExitSign.Normalize();

	// Calculate the dot product between the agent's forward direction and the vector from the agent to the exit sign
	float DotProduct = FVector::DotProduct(AgentForward, AgentToExitSign);

	// If the dot product is negative, the exit sign is behind the agent
	return DotProduct < 0.0f;
}

void ACFAgent::MoveDownStair(ACrowdFlowExitStaircase* Staircase)
{
	return;
	if (!Staircase || CurrentStaircase == Staircase)
	{
		return;
	}

	CurrentStaircase = Staircase;

	MoveToLocation(FinalDestination);
}