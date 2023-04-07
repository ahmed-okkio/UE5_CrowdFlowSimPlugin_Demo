// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowFinalDestination.h"
#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"


// Sets default values
ACrowdFlowFinalDestination::ACrowdFlowFinalDestination()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard Icon"));
	SpriteComponent->SetupAttachment(RootComponent);
	SpriteComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

}

// Called when the game starts or when spawned
void ACrowdFlowFinalDestination::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACrowdFlowFinalDestination::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

