// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowExit.h"
#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"


// Sets default values
ACrowdFlowExit::ACrowdFlowExit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard Icon"));
	SpriteComponent->SetupAttachment(RootComponent);
	SpriteComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

}

// Called when the game starts or when spawned
void ACrowdFlowExit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACrowdFlowExit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

