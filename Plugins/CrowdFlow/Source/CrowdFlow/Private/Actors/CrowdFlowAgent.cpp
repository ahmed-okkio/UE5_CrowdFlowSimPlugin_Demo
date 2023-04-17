// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowAgent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Actors/CrowdFlowExitSign.h"
#include "Actors/CrowdFlowFinalDestination.h"
#include "Actors/CrowdFlowExitStaircase.h"
#include "Kismet/GameplayStatics.h"
#include "CrowdFlowSimulationState.h"
#include "GameMode/CrowdFlowGameMode.h"

float ACrowdFlowAgent::SphereRadius = 100.f;

// Sets default values
ACrowdFlowAgent::ACrowdFlowAgent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	RootComponent = SphereComponent;
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	UStaticMesh* SphereMesh = SphereMeshAsset.Object;
	SphereComponent->SetStaticMesh(SphereMesh);
	SphereComponent->GetStaticMesh()->SetExtendedBounds(FBoxSphereBounds(FVector::ZeroVector, FVector(SphereRadius), SphereRadius));

	
	AgentData.AgentName = GetActorNameOrLabel();
}

void ACrowdFlowAgent::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<ACrowdFlowGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnSimulationStart.AddDynamic(this, &ACrowdFlowAgent::StartSimulating);
	}
}

void ACrowdFlowAgent::StartSimulating()
{
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowFinalDestination::StaticClass(), AllActors);

	if (AllActors[0])
	{
		FinalDestination = AllActors[0]->GetActorLocation();
	}

	NearestExitLocation = GetNearestExitLocation();

	if (ACrowdFlowSimulationState* SimState = Cast<ACrowdFlowSimulationState>(GetWorld()->GetGameState()))
	{
		AgentData.StartTime = SimState->GetTimeInHMS();
	}


	BeginLookingForDirectMoveToFinalDestination();
	CalculateNextMove();
	ExecuteNextMove();
}

bool ACrowdFlowAgent::IsGrounded()
{
	FHitResult Hit;
	FVector CurrentLocation = GetActorLocation();
	FVector EndLocation = CurrentLocation + FVector(0,0,-1) * (SphereRadius+5.f);
	GetWorld()->LineTraceSingleByChannel(Hit, CurrentLocation, EndLocation, ECollisionChannel::ECC_PhysicsBody);
	return Hit.bBlockingHit;
}

bool ACrowdFlowAgent::IsFinalDestinationVisible() const
{
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), FinalDestination, ECollisionChannel::ECC_GameTraceChannel2);
	return !Hit.bBlockingHit;
}

bool ACrowdFlowAgent::IsExitOnSameFloor(FVector ExitLocation) const
{
	float CurrentHeight = GetActorLocation().Z;
	float LowerBound = ExitLocation.Z - SameFloorHeightMargin;
	float UpperBound = ExitLocation.Z + SameFloorHeightMargin;

	return (CurrentHeight >= LowerBound && CurrentHeight <= UpperBound);
}

void ACrowdFlowAgent::AttemptDirectMoveToFinalDestination()
{
	if (IsFinalDestinationVisible() && IsExitOnSameFloor(FinalDestination))
	{
		GetWorld()->GetTimerManager().ClearTimer(TH_DirectMove);

		NextMove = FMove();

		NextMove.Direction = FinalDestination - GetActorLocation();
		NextMove.Direction.Normalize();
		NextMove.Units = FVector::Distance(FinalDestination, GetActorLocation());

		ClearMoveQueue();
		ExecuteNextMove();

		FoundDirectMoveToExit = true;
	}
	else
	{
		FoundDirectMoveToExit = false;
	}
}

void ACrowdFlowAgent::BeginLookingForDirectMoveToFinalDestination()
{
	GetWorld()->GetTimerManager().SetTimer(TH_DirectMove, this, &ACrowdFlowAgent::AttemptDirectMoveToFinalDestination, DirectMoveSearchRate, true);
}

void ACrowdFlowAgent::CalculateNextMove()
{
	NextMove = FMove();
	
	FVector ForwardVector = GetActorForwardVector();
	float DegreesBetweenMove = 360 / TurnSmoothness;
	for (int32 i = 0; i < 360; i += DegreesBetweenMove)
	{
		FMove NewMove;
		FVector NewDirection = ForwardVector.RotateAngleAxis(i, FVector(0, 0, 1));
		NewMove.Direction = NewDirection;
		NewMove.Units = UnitsPerMove;

		if (IsBestMove(NewMove))
		{
			NextMove = NewMove;
		}
	}
}


bool ACrowdFlowAgent::IsBestMove(FMove NewMove) const
{
	
	FVector CurrentLocation = GetActorLocation();

	FVector BestMoveLocation = CurrentLocation + NextMove.Direction * (NextMove.Units + SphereRadius);
	FVector NewMoveLocation = CurrentLocation + NewMove.Direction * (NewMove.Units + SphereRadius);

	float BestMoveDistance = FVector::Distance(BestMoveLocation, NearestExitLocation);
	float NewMoveDistance = FVector::Distance(NewMoveLocation, NearestExitLocation);

	
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, CurrentLocation, NewMoveLocation, ECollisionChannel::ECC_PhysicsBody);
	DrawDebugLine(GetWorld(), CurrentLocation, NewMoveLocation, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false);

	if (Hit.bBlockingHit)
	{
		return false;
	}

	if (NextMove.Units == 0)
	{
		DrawDebugLine(GetWorld(), CurrentLocation, NewMoveLocation, FColor::Purple, false);
		return true;

	}

	if (NewMoveDistance > BestMoveDistance)
	{
		return false;
	}

	DrawDebugLine(GetWorld(), CurrentLocation, NewMoveLocation, FColor::Green, false);

	return true;
}

void ACrowdFlowAgent::ExecuteNextMove()
{
	MoveTowardsDirection(NextMove.Direction, NextMove.Units);
}

void ACrowdFlowAgent::ClearMoveQueue()
{
	// Move system overhaul
	//MoveExecutionQ.Empty();
	CurrentUnitsLeft = 0;
	ExitSignBeingFollowed = nullptr;
	MovementBlockedDelegate.RemoveAll(this);
	MovementFinishedDelegate.RemoveAll(this);
	GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
}

void ACrowdFlowAgent::MoveTowardsDirection(FVector Direction, int32 Units)
{
	if (GetDistanceToFinalDestination() < ExitReachedRange)
	{
		return GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
	}

	if (Units == 0)
	{
		return;
	}

	CurrentUnitsLeft = Units;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "MoveTillUnitAmount", Direction, Units);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
}

void ACrowdFlowAgent::MoveToLocation(const FVector Destination)
{
	FVector CurrentLocation = GetActorLocation();
	int32 Units = (int32) FVector::Distance(CurrentLocation, Destination);
	FVector Direction = (Destination - CurrentLocation);
	Direction.Normalize();
	CurrentUnitsLeft = Units;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "MoveTillUnitAmount", Direction, Units);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
}

void ACrowdFlowAgent::MoveToExit(ACrowdFlowExitSign* ExitSign)
{
	// Don't move to this sign if we have already moved to it before.
	if (ExitSign == ExitSignBeingFollowed || ExitSign == LastFollowedExitSign)
	{
		return;
	}

	if (ExitSignBeingFollowed)
	{
		// If there is a sign already being followed unfollow
		ExitSignBeingFollowed->UnfollowSign(this);
		// Clean up cause of interrupting movement delegate
		MovementFinishedDelegate.RemoveDynamic(this, &ACrowdFlowAgent::OnLeftExit);

	}

	ExitSignBeingFollowed = ExitSign;
	ExitSignBeingFollowed->FollowSign(this);

	if (ExitSignBeingFollowed)
	{
		FVector ExitDestination = ExitSignBeingFollowed->GetExitSignDestination();
		ExitDestination.Z = GetActorLocation().Z;
		//MoveToLocation(ExitDestination);

		FVector CurrentLocation = GetActorLocation();
		int32 Units = (int32)FVector::Distance(CurrentLocation, ExitDestination);
		FVector Direction = (ExitDestination - CurrentLocation);
		Direction.Normalize();
		CurrentUnitsLeft = Units;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "MoveTillUnitAmount", Direction);
		GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
		GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
		MovementFinishedDelegate.AddUniqueDynamic(this, &ACrowdFlowAgent::OnReachedExit);
	}

}

void ACrowdFlowAgent::MoveTillUnitAmount(FVector Direction)
{
	if (GameMode)
	{
		if (!GameMode->IsSimulationStarted())
		{
			return;
		}
	}

	FVector NewMoveLocation = GetActorLocation() + Direction * (PersonalSpace + SphereRadius);

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), NewMoveLocation, ECollisionChannel::ECC_GameTraceChannel1);
	if (Hit.bBlockingHit)
	{
		if (ACrowdFlowAgent* HitActor = Cast<ACrowdFlowAgent>(Hit.GetActor()))
		{
			DrawDebugLine(GetWorld(), GetActorLocation(), Hit.Location, FColor::Red, false);
			SphereComponent->SetAllPhysicsAngularVelocityInDegrees(FVector(0, 0, 0));

			// Possibly add timer to attempt to go around if agent doesn't move out of the way
			return;
		}
	}

	if (CurrentUnitsLeft > 0)
	{
		SetActorRotation(Direction.Rotation());
		SetActorLocation(GetActorLocation() + (Direction * 1));
		FVector T = FVector(0, 0, 0);
		T.Z = GetVelocity().Z;
		SphereComponent->SetAllPhysicsLinearVelocity(T, false);
		CurrentUnitsLeft--;
		TotalUnits++;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TH_Movement);

		if (ExitSignBeingFollowed)
		{
			MovementFinishedDelegate.Broadcast();
		}
		else
		{
			CalculateNextMove();
			ExecuteNextMove();
		}
		
		if (FoundDirectMoveToExit)
		{
			if (ACrowdFlowSimulationState* SimState = Cast<ACrowdFlowSimulationState>(GetWorld()->GetGameState()))
			{
				AgentData.EndTime = SimState->GetTimeInHMS();
				AgentData.Duration = AgentData.GetEvacuationDuration();
				AgentData.UnitsTraveled = TotalUnits;
				AgentData.AverageUnitsPerSecond = TotalUnits / AgentData.GetDurationInSeconds();
				SimState->SubmitAgentData(AgentData);
				Destroy();
			}

		}

	}
}

void ACrowdFlowAgent::MoveTillBlocked(FVector Direction)
{
	if (GameMode)
	{
		if (!GameMode->IsSimulationStarted())
		{
			return;
		}
	}

	if (CurrentUnitsLeft > 0)
	{
		SetActorRotation(Direction.Rotation());
		SetActorLocation(GetActorLocation() + Direction * 1);
		FVector T = FVector(0,0,0);
		T.Z = GetVelocity().Z;
		SphereComponent->SetAllPhysicsLinearVelocity(T, false);
		CurrentUnitsLeft--;
		TotalUnits++;
	}
	else
	{
		FVector NewMoveLocation = GetActorLocation() + Direction * (PersonalSpace + SphereRadius);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), NewMoveLocation, ECollisionChannel::ECC_WorldStatic);
		if (Hit.bBlockingHit)
		{
			if (ACrowdFlowAgent* HitActor = Cast<ACrowdFlowAgent>(Hit.GetActor()))
			{
				return;
			}

			DrawDebugLine(GetWorld(), GetActorLocation(), NewMoveLocation, FColor::Red,false);

			GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
			MovementBlockedDelegate.Broadcast();
		}
		else
		{
			DrawDebugLine(GetWorld(), GetActorLocation(), NewMoveLocation, FColor::Blue,false,1);

			CurrentUnitsLeft = PersonalSpace;
		}

	}
}

void ACrowdFlowAgent::OnReachedExit()
{
	MovementFinishedDelegate.RemoveDynamic(this, &ACrowdFlowAgent::OnReachedExit);
	
	if (!ExitSignBeingFollowed)
	{
		FText();
		return;
	}

	FVector Direction = -ExitSignBeingFollowed->GetActorForwardVector();
	CurrentUnitsLeft = UnitsToMovePastExit;

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "MoveTillUnitAmount", Direction);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
	MovementFinishedDelegate.AddUniqueDynamic(this, &ACrowdFlowAgent::OnLeftExit);

}

void ACrowdFlowAgent::OnLeftExit()
{
	MovementFinishedDelegate.RemoveDynamic(this, &ACrowdFlowAgent::OnLeftExit);

	LastFollowedExitSign = ExitSignBeingFollowed;
	ExitSignBeingFollowed->UnfollowSign(this);
	ExitSignBeingFollowed = nullptr;
}

// Called every frame
void ACrowdFlowAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FoundDirectMoveToExit)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), FinalDestination, FColor::Red, false);
	}
}

void ACrowdFlowAgent::MoveDownStair(ACrowdFlowExitStaircase* Staircase, bool RightStaircase)
{
	if (!Staircase || CurrentStaircase == Staircase)
	{
		return;
	}

	CurrentStaircase = Staircase;
	ClearMoveQueue();

	if (RightStaircase)
	{
		MoveDownRightStair();
	}
	else
	{
		MoveDownLeftStair();
	}

}

void ACrowdFlowAgent::MoveDownRightStair()
{
	FindRightMostWall();
}

void ACrowdFlowAgent::FindRightMostWall()
{
	FVector DirectionToRight = GetActorForwardVector().RotateAngleAxis(90.f, FVector(0, 0, 1));
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "MoveTillBlocked", DirectionToRight);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);

	// Bind function to trigger when right most wall is found
	MovementBlockedDelegate.AddDynamic(this, &ACrowdFlowAgent::OnFoundRightMostWall);
}

void ACrowdFlowAgent::FollowRightMostWall()
{
	// Move along right wall
	FVector DirectionToStair = GetActorForwardVector().RotateAngleAxis(-90.f, FVector(0, 0, 1));

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "MoveTillBlocked", DirectionToStair);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
}

void ACrowdFlowAgent::MoveDownLeftStair()
{
	FindLeftMostWall();
}

void ACrowdFlowAgent::FindLeftMostWall()
{
	FVector DirectionToLeft = GetActorForwardVector().RotateAngleAxis(-90.f, FVector(0, 0, 1));
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "MoveTillBlocked", DirectionToLeft);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);

	// Bind function to trigger when left most wall is found
	MovementBlockedDelegate.AddDynamic(this, &ACrowdFlowAgent::OnFoundLeftMostWall);
}

void ACrowdFlowAgent::FollowLeftMostWall()
{
	// Move along left wall
	FVector DirectionToStair = GetActorForwardVector().RotateAngleAxis(90.f, FVector(0, 0, 1));
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "MoveTillBlocked", DirectionToStair);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
}

void ACrowdFlowAgent::OnFoundRightMostWall()
{
	FollowRightMostWall();
}

void ACrowdFlowAgent::SeeExitSign(ACrowdFlowExitSign* ExitSign)
{
	if (!ExitSign || FoundDirectMoveToExit)
	{
		return;
	}

	if (ExitSign->IsKnownExit())
	{
		MoveToExit(ExitSign);
	}
}

FVector ACrowdFlowAgent::GetNearestExitLocation()
{
	TArray<AActor*> Exits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowExitSign::StaticClass(), Exits);
	ACrowdFlowExitSign* NearestExit = nullptr;
	for (auto Exit : Exits)
	{
		ACrowdFlowExitSign* ExitSign  = Cast<ACrowdFlowExitSign>(Exit);
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

void ACrowdFlowAgent::OnFoundLeftMostWall()
{
	FollowLeftMostWall();
}

float ACrowdFlowAgent::GetSphereRadius()
{
	return SphereRadius;
}

int32 ACrowdFlowAgent::GetCurrentUnitsLeft()
{
	return CurrentUnitsLeft;
}

int32 ACrowdFlowAgent::GetDistanceToFinalDestination()
{
	return FVector::Distance(GetActorLocation(), FinalDestination);
}
