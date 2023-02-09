// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowAgent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Actors/CrowdFlowExitSign.h"
#include "Actors/CrowdFlowExit.h"
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
	Delegate.BindUFunction(this, "UpdateMovement", Direction, Units);
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
	Delegate.BindUFunction(this, "UpdateMovement", Direction, Units);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
}

void ACrowdFlowAgent::MoveToExitSign(ACrowdFlowExitSign* ExitSign)
{
	// Don't move to this sign if we have already moved to it before.
	if (ExitSign == VisibleExitSign)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
	VisibleExitSign = ExitSign;

	if (VisibleExitSign)
	{
		FVector ExitDestination = VisibleExitSign->GetExitSignDestination();
		ExitDestination.Z = GetActorLocation().Z;
		MoveToLocation(ExitDestination);

		FVector CurrentLocation = GetActorLocation();
		int32 Units = (int32)FVector::Distance(CurrentLocation, ExitDestination);
		FVector Direction = (ExitDestination - CurrentLocation);
		Direction.Normalize();
		CurrentUnitsLeft = Units - 5;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "UpdateMovement", Direction, Units);
		GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
	}

}

void ACrowdFlowAgent::UpdateMovement(FVector Direction, int32 Units)
{

	if (CurrentUnitsLeft > 0)
	{
		SphereComponent->SetWorldLocation(SphereComponent->GetComponentLocation() + Direction * 1);
		CurrentUnitsLeft--;

	}
	else
	{
		//VisibleExitSign = nullptr;
		GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
		CalculateNextMove();
		ExecuteNextMove();
	}
}

// Called every frame
void ACrowdFlowAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//MoveTowardsDirection(SphereComponent->GetForwardVector(), 5);

}

int32 ACrowdFlowAgent::GetCurrentUnitsLeft()
{
	return CurrentUnitsLeft;
}

int32 ACrowdFlowAgent::GetDistanceToExit()
{
	return FVector::Distance(GetActorLocation(), ExitLocation);
}


