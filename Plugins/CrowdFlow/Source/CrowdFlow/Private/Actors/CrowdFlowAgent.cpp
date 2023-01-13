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
		SphereComponent->SetStaticMesh(StaticMesh);
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
	MoveTowardsDirection(SphereComponent->GetForwardVector(), 5);
	
}

void ACrowdFlowAgent::MoveTowardsDirection(FVector Direction, int32 Units)
{
	float Speed = 0.001;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "UpdateMovement", Direction, Units);
	GetWorld()->GetTimerManager().SetTimer(TH_Movement, Delegate, Speed, true);
}

void ACrowdFlowAgent::CalculatePossibleMoves()
{
}

void ACrowdFlowAgent::UpdateMovement(FVector Direction, int32 Units)
{
	if (Units > 0)
	{
		SphereComponent->SetWorldLocation(SphereComponent->GetComponentLocation() + Direction * 1);
		Units--;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TH_Movement);
	}
}

// Called every frame
void ACrowdFlowAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//MoveTowardsDirection(SphereComponent->GetForwardVector(), 5);

}

int32 ACrowdFlowAgent::GetDistanceToExit()
{
	return FVector::Distance(GetActorLocation(), ExitLocation);
}

