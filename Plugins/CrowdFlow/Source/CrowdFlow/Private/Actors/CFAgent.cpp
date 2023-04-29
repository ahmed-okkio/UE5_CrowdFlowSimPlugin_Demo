// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CFAgent.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/CrowdFlowFinalDestination.h"

ACFAgent::ACFAgent()
{
}

void ACFAgent::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> AllActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowFinalDestination::StaticClass(), AllActors);


	if (AllActors[0])
	{
		FinalDestination = AllActors[0]->GetActorLocation();
	}

}

FVector ACFAgent::GetFinalDestination() const
{
	return FinalDestination;
}
