// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowAgent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Actors/CrowdFlowExitSign.h"
#include "Actors/CrowdFlowExit.h"
#include "Actors/CrowdFlowExitStaircase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACrowdFlowAgent::ACrowdFlowAgent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

		SphereComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
		RootComponent = SphereComponent;
		static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
		SphereComponent->SetStaticMesh(SphereMeshAsset.Object);
		
	// attach spherecomponent to root
}

// Called when the game starts or when spawned
void ACrowdFlowAgent::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowExit::StaticClass(), AllActors);
	
	if (AllActors[0])
	{
		//ExitLocation = AllActors[0]->GetActorLocation();
		ExitLocation = FVector(-2500.000000, -496.286185, 1143.345423);
	}
	SphereRadius = SphereComponent->GetStaticMesh()->GetBounds().SphereRadius;
	
	BeginLookingForDirectMoveToExit();
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

void ACrowdFlowAgent::LookForPathAround()
{

}

bool ACrowdFlowAgent::IsExitVisible() const
{
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), ExitLocation, ECollisionChannel::ECC_GameTraceChannel2);
	return !Hit.bBlockingHit;
}

bool ACrowdFlowAgent::IsExitOnSameFloor() const
{
	float CurrentHeight = GetActorLocation().Z;
	float LowerBound = ExitLocation.Z - SameFloorHeightMargin;
	float UpperBound = ExitLocation.Z + SameFloorHeightMargin;

	return (CurrentHeight >= LowerBound && CurrentHeight <= UpperBound);
}

void ACrowdFlowAgent::AttemptDirectMoveToExit()
{
	if (IsExitVisible() && IsExitOnSameFloor())
	{
		FoundDirectMoveToExit = true;

		NextMove = FMove();

		NextMove.Direction = ExitLocation - GetActorLocation();
		NextMove.Direction.Normalize();
		NextMove.Units = FVector::Distance(ExitLocation, GetActorLocation());

		ClearMoveQueue();
		ExecuteNextMove();
	}
	else
	{
		FoundDirectMoveToExit = false;
	}
}

void ACrowdFlowAgent::BeginLookingForDirectMoveToExit()
{
	GetWorld()->GetTimerManager().SetTimer(TH_DirectMove, this, &ACrowdFlowAgent::AttemptDirectMoveToExit, DirectMoveSearchRate, true);
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

	float BestMoveDistance = FVector::Distance(BestMoveLocation, ExitLocation);
	float NewMoveDistance = FVector::Distance(NewMoveLocation, ExitLocation);

	
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, CurrentLocation, NewMoveLocation, ECollisionChannel::ECC_PhysicsBody);
	//DrawDebugLine(GetWorld(), CurrentLocation, NewMoveLocation, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, -1, 10.0f);

	if (Hit.bBlockingHit)
	{
		return false;
	}

	if (NextMove.Units == 0)
	{
		//DrawDebugLine(GetWorld(), CurrentLocation, NewMoveLocation, FColor::Purple, false, 5.0f, -1, 10.0f);
		return true;

	}

	if (NewMoveDistance > BestMoveDistance)
	{
		return false;
	}

	//DrawDebugLine(GetWorld(), CurrentLocation, NewMoveLocation, FColor::Green, false, 5.0f, -2, 10.0f);

	return true;
}

void ACrowdFlowAgent::CalculateNextMicroMove()
{
	// Move system overhall
	/*if (!ActiveMacroMove)
	{
		return;
	}

	FVector Heuristic = ActiveMacroMove->Direction;
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
	}*/
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
	MovementBlockedDelegate.RemoveAll(this);
	MovementFinishedDelegate.RemoveAll(this);
	GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
}

void ACrowdFlowAgent::MoveTowardsDirection(FVector Direction, int32 Units)
{
	if (GetDistanceToExit() < ExitReachedRange)
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
		FVector ExitDestination = ExitSignBeingFollowed->GetActorLocation();
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
	if (CurrentUnitsLeft > 0)
	{
		SetActorRotation(Direction.Rotation());
		SetActorLocation(GetActorLocation() + (Direction * 1));
		FVector T = FVector(0, 0, 0);
		T.Z = GetVelocity().Z;
		SphereComponent->SetAllPhysicsLinearVelocity(T, false);
		CurrentUnitsLeft--;
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

	}
}

void ACrowdFlowAgent::MoveTillBlocked(FVector Direction)
{
	if (CurrentUnitsLeft > 0)
	{
		SetActorRotation(Direction.Rotation());
		SetActorLocation(GetActorLocation() + Direction * 1);
		FVector T = FVector(0,0,0);
		T.Z = GetVelocity().Z;
		SphereComponent->SetAllPhysicsLinearVelocity(T, false);
		CurrentUnitsLeft--;

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

			DrawDebugLine(GetWorld(), GetActorLocation(), NewMoveLocation, FColor::Red,false,1);

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
	//LastFollowedExitSign = ExitSignBeingFollowed;

	//FVector Destination = GetActorLocation() + (-ExitSignBeingFollowed->GetActorForwardVector() * UnitsToMovePastExit);

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
	//MoveTowardsDirection(SphereComponent->GetForwardVector(), 5);
	if (FoundDirectMoveToExit)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), ExitLocation, FColor::Red, false);
	}
	
	// Move system overhaul
	/*if (ActiveMacroMove == nullptr) 
	{
		MacroMoveQ.Dequeue(ActiveMacroMove OUT);
	}*/

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

void ACrowdFlowAgent::ExecuteActiveMove()
{

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

void ACrowdFlowAgent::SeeExit(ACrowdFlowExitSign* ExitSign)
{
	if (!ExitSign)
	{
		return;
	}

	if (ExitSign->IsKnownExit())
	{
		MoveToExit(ExitSign);
	}
}

void ACrowdFlowAgent::OnFoundLeftMostWall()
{
	FollowLeftMostWall();
}

int32 ACrowdFlowAgent::GetCurrentUnitsLeft()
{
	return CurrentUnitsLeft;
}

int32 ACrowdFlowAgent::GetDistanceToExit()
{
	return FVector::Distance(GetActorLocation(), ExitLocation);
}


