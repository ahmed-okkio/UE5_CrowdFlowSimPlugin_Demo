// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowAgent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACrowdFlowAgent::ACrowdFlowAgent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

		SphereComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
		RootComponent = SphereComponent;
		//SphereComponent->SetStaticMesh(StaticMesh);
		static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
		SphereComponent->SetStaticMesh(SphereMeshAsset.Object);
		
	// attach spherecomponent to root
}

// Called when the game starts or when spawned
void ACrowdFlowAgent::BeginPlay()
{
	Super::BeginPlay();
	if (StaticMesh)
	{
	}
	CalculatePossibleMoves();
	MoveTowardsBestMove();
	//MoveTowardsDirection(SphereComponent->GetForwardVector(), 5);
	
}

void ACrowdFlowAgent::MoveTowardsDirection(FVector Direction, int32 Units)
{
	if (GetDistanceToExit() == 0)
	{
		return;
	}

	if (Units == 0)
	{
		return;
	}

	float Speed = 0.001;
	CurrentUnitsLeft = Units;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "UpdateMovement", Direction, Units);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
}

void ACrowdFlowAgent::MoveTowardsBestMove()
{
	MoveTowardsDirection(BestMove.Direction, BestMove.Units);
}

void ACrowdFlowAgent::CalculatePossibleMoves()
{
	FVector ForwardVector = GetActorForwardVector();
	float DegreesBetweenMove = 360 / TurnSmoothness;
	bool NoNewMoves = true;
	for (int32 i = 0; i < 360; i += DegreesBetweenMove)
	{
		FMove NewMove;
		FVector NewDirection = ForwardVector.RotateAngleAxis(i, FVector(0, 0, 1));
		NewMove.Direction = NewDirection;
		NewMove.Units = UnitsPerMove;

		if (IsBestMove(NewMove))
		{
			BestMove = NewMove;
			NoNewMoves = false;
		}

		PossibleMoves.Add(NewMove);
	}
	if (NoNewMoves)
	{
		BestMove = FMove();
	}
}

bool ACrowdFlowAgent::IsBestMove(FMove NewMove) const
{
	
	FVector CurrentLocation = SphereComponent->GetComponentLocation();

	FVector BestMoveLocation = CurrentLocation + BestMove.Direction * BestMove.Units;
	FVector NewMoveLocation = CurrentLocation + NewMove.Direction * NewMove.Units;

	float BestMoveDistance = FVector::Distance(BestMoveLocation, ExitLocation);
	float NewMoveDistance = FVector::Distance(NewMoveLocation, ExitLocation);

	
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, CurrentLocation, NewMoveLocation, ECollisionChannel::ECC_PhysicsBody);
	DrawDebugLine(GetWorld(), CurrentLocation, NewMoveLocation, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, -1, 10.0f);

	if (Hit.bBlockingHit)
	{
		return false;
	}

	if (BestMove.Units == 0)
	{
		return true;
	}

	if (NewMoveDistance > BestMoveDistance)
	{
		return false;
	}


	return true;
}

void ACrowdFlowAgent::SelectBestMove()
{

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
		GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
		CalculatePossibleMoves();
		MoveTowardsBestMove();
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


