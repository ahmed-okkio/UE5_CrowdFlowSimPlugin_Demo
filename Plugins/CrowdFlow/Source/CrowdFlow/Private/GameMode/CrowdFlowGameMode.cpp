// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/CrowdFlowGameMode.h"
#include "CrowdFlowSimulationState.h"

void ACrowdFlowGameMode::InitGameState()
{
    Super::InitGameState();

    SimState = Cast<ACrowdFlowSimulationState>(GameState);
}

void ACrowdFlowGameMode::StartPlay()
{
    Super::StartPlay();
    
}

void ACrowdFlowGameMode::BeginPlay()
{
    Super::BeginPlay();
    if (StartSimulationOnBeginPlay)
    {
        StartSimulation();
    }
}

void ACrowdFlowGameMode::StartSimulation()
{
    // Set the simulation started flag
    bIsSimulationStarted = true;

    // Broadcast the simulation start event
    OnSimulationStart.Broadcast();

    if (SimState)
    {
       SimState->ClearTimer();
       SimState->StartTimer();
    }
}

void ACrowdFlowGameMode::StopSimulation()
{
    if (SimState)
    {
        SimState->StopTimer();
    }
}

FSimulationStartDelegate ACrowdFlowGameMode::GetSimulationStartDelegate()
{
    return OnSimulationStart;
}

bool ACrowdFlowGameMode::IsSimulationStarted() const
{
    return bIsSimulationStarted;
}
