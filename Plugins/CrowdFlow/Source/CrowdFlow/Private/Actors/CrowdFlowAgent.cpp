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
		ExitLocation = AllActors[0]->GetActorLocation();
	}
	SphereRadius = SphereComponent->GetStaticMesh()->GetBounds().SphereRadius;
	
	BeginLookingForDirectMoveToExit();
	CalculateNextMove();
	ExecuteNextMove();
}

bool ACrowdFlowAgent::IsExitVisible() const
{
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), ExitLocation, ECollisionChannel::ECC_WorldStatic);
	return !Hit.bBlockingHit;
}

void ACrowdFlowAgent::AttemptDirectMoveToExit()
{
	if (IsExitVisible())
	{
		NextMove = FMove();

		NextMove.Direction = ExitLocation - GetActorLocation();
		NextMove.Direction.Normalize();
		NextMove.Units = FVector::Distance(ExitLocation, GetActorLocation());

		GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
		ExecuteNextMove();
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

		PossibleMoves.Add(NewMove);
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

void ACrowdFlowAgent::ExecuteNextMove()
{
	MoveTowardsDirection(NextMove.Direction, NextMove.Units);
}

void ACrowdFlowAgent::MoveTowardsDirection(FVector Direction, int32 Units)
{
	if (GetDistanceToExit() < ExitReachedRange)
	{
		return;
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
	if (ExitSign == VisibleExitSign || ExitSign == LastVisibleExitSign)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
	VisibleExitSign = ExitSign;

	if (VisibleExitSign)
	{
		FVector ExitDestination = VisibleExitSign->GetActorLocation();
		ExitDestination.Z = GetActorLocation().Z;
		MoveToLocation(ExitDestination);

		FVector CurrentLocation = GetActorLocation();
		int32 Units = (int32)FVector::Distance(CurrentLocation, ExitDestination);
		FVector Direction = (ExitDestination - CurrentLocation);
		Direction.Normalize();
		CurrentUnitsLeft = Units;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "MoveTillUnitAmount", Direction);
		GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
		MovementFinishedDelegate.AddDynamic(this, &ACrowdFlowAgent::OnReachedExit);
	}

}

void ACrowdFlowAgent::MoveTillUnitAmount(FVector Direction)
{

	if (CurrentUnitsLeft > 0)
	{
		SetActorRotation(Direction.Rotation());

		SetActorLocation(GetActorLocation() + Direction * 1);
		CurrentUnitsLeft--;

	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TH_Movement);

		if (VisibleExitSign)
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
		CurrentUnitsLeft--;

	}
	else
	{
			
	}
	{
		FVector NewMoveLocation = GetActorLocation() + Direction * (PersonalSpace + SphereRadius);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), NewMoveLocation, ECollisionChannel::ECC_PhysicsBody);
		if (Hit.bBlockingHit)
		{
			GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
		}
		else
		{
			CurrentUnitsLeft = PersonalSpace;
		}

	}
}

void ACrowdFlowAgent::OnReachedExit()
{
	MovementFinishedDelegate.RemoveDynamic(this, &ACrowdFlowAgent::OnReachedExit);
	
	//LastVisibleExitSign = VisibleExitSign;

	CurrentUnitsLeft = UnitsToMovePastExit;
	//FVector Destination = GetActorLocation() + (-VisibleExitSign->GetActorForwardVector() * UnitsToMovePastExit);

	FVector Direction = -VisibleExitSign->GetActorForwardVector();

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "MoveTillUnitAmount", Direction);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);

	LastVisibleExitSign = VisibleExitSign;
	VisibleExitSign = nullptr;
}

// Called every frame
void ACrowdFlowAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//MoveTowardsDirection(SphereComponent->GetForwardVector(), 5);

}

void ACrowdFlowAgent::MoveDownStair(ACrowdFlowExitStaircase* Staircase, bool RightStaircase)
{
	if (!Staircase || CurrentStaircase == Staircase)
	{
		return;
	}

	CurrentStaircase = Staircase;
	GetWorld()->GetTimerManager().ClearTimer(TH_Movement);

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
	// Find rightmost wall
	FindRightMostWall();
	// Walk straight and keep hugging right wall



}

void ACrowdFlowAgent::FindRightMostWall()
{
	FVector DirectionToRight = GetActorForwardVector().RotateAngleAxis(90.f, FVector(0, 0, 1));

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "MoveTillBlocked", DirectionToRight);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
	MovementBlockedDelegate.AddDynamic(this, &ACrowdFlowAgent::OnFoundRightMostWall);
}

void ACrowdFlowAgent::OnFoundRightMostWall()
{
	MovementBlockedDelegate.RemoveDynamic(this, &ACrowdFlowAgent::OnFoundRightMostWall);


}

void ACrowdFlowAgent::MoveDownLeftStair()
{
}

int32 ACrowdFlowAgent::GetCurrentUnitsLeft()
{
	return CurrentUnitsLeft;
}

int32 ACrowdFlowAgent::GetDistanceToExit()
{
	return FVector::Distance(GetActorLocation(), ExitLocation);
}


