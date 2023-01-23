// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowExitSign.h"
//#include "Kismet/KismetSystemLibrary.h"
#include "Engine/TriggerVolume.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"


// Sets default values
ACrowdFlowExitSign::ACrowdFlowExitSign()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoundingBox = FVector(1, 1, 1);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//TriggerVolume = CreateDefaultSubobject<ATriggerVolume>(TEXT("TriggerVolume"));
	//TriggerVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	//TriggerBoxBrush = TriggerVolume->Brush;
	//TriggerVolume->Brush = TriggerBoxBrush;
}

// Called when the game starts or when spawned
void ACrowdFlowExitSign::BeginPlay()
{
	Super::BeginPlay();
	FVector StartLocation = GetActorLocation() + (GetActorForwardVector() * 100);
	FVector EndLocation = GetActorForwardVector() * 10000	;
	FHitResult Hit;
	//GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), EndLocation, ECollisionChannel::ECC_PhysicsBody);
	//DrawDebugLine(GetWorld(), GetActorLocation(), EndLocation, FColor::Red, false, 5.0f, -1, 10.0f);
	ForwardArrow = FindComponentByClass<UArrowComponent>();
	if (ForwardArrow)
	{
		FVector Center = GetActorLocation() + ForwardArrow->GetForwardVector() * BoundingBox.Y;
		DrawDebugBox(GetWorld(), Center, BoundingBox, FColor::Green, true);
	}
	//GetWorld()->BoxTraceByCHannel

		
}

void ACrowdFlowExitSign::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	

}

#if WITH_EDITOR
void ACrowdFlowExitSign::PostEditMove(bool bFinished)
{
	// Only call this a single time when an actor is first placed (moved) in a level
	if (false == PostEditMoveHasBeenCalled)
	{
		// The stuff you want to do when you place an actor in the level goes here

		PostEditMoveHasBeenCalled = true;
	}
	Super::PostEditMove(bFinished);
	if (!TriggerVolume)
	{
		TriggerVolume = GetWorld()->SpawnActor<ATriggerVolume>(ATriggerVolume::StaticClass(), FActorSpawnParameters());
	}

	ForwardArrow = FindComponentByClass<UArrowComponent>();
	if (ForwardArrow)
	{
		ForwardArrow->GetForwardVector();
	}
}
#endif

// Called every frame
void ACrowdFlowExitSign::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

