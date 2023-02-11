// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ExitStaircase.h"

// Sets default values
AExitStaircase::AExitStaircase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExitStaircase::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AExitStaircase::ShouldTickIfViewportsOnly() const
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

// Called every frame
void AExitStaircase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasActorBegunPlay() && DrawDetectionDebug)
	{
		FVector Center = GetActorLocation();
		//Center.Z += BoundingBox.Z;
		DrawDebugBox(GetWorld(), Center, BoundingBox, GetActorRotation().Quaternion(), FColor::Green, false);
	}

}

