// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowExitSign.h"
//#include "Kismet/KismetSystemLibrary.h"
#include "Engine/TriggerVolume.h"
#include "Components/SceneComponent.h"

// Sets default values
ACrowdFlowExitSign::ACrowdFlowExitSign()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	TriggerVolume = CreateDefaultSubobject<ATriggerVolume>(TEXT("TriggerVolume"));
	TriggerVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	 

}

// Called when the game starts or when spawned
void ACrowdFlowExitSign::BeginPlay()
{
	Super::BeginPlay();
	FVector StartLocation = GetActorLocation() + (GetActorForwardVector() * 100);
	FVector EndLocation = GetActorForwardVector() * 10000	;
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), EndLocation, ECollisionChannel::ECC_PhysicsBody);
	DrawDebugLine(GetWorld(), GetActorLocation(), EndLocation, FColor::Red, false, 5.0f, -1, 10.0f);
	//GetWorld()->BoxTraceByCHanne;l

	
}

// Called every frame
void ACrowdFlowExitSign::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

