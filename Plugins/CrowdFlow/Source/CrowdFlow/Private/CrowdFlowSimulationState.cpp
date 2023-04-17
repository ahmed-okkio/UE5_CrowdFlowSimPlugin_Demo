// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdFlowSimulationState.h"

void ACrowdFlowSimulationState::HandleBeginPlay()
{
    Super::HandleBeginPlay();
  
}



void ACrowdFlowSimulationState::StartTimer()
{

	// Set up a timer with a delegate
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ACrowdFlowSimulationState::TickTimer, 1.0f, true);

}

void ACrowdFlowSimulationState::TickTimer()
{
	TimerInSeconds += 1.f;
}

void ACrowdFlowSimulationState::StopTimer()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void ACrowdFlowSimulationState::ClearTimer()
{
	TimerInSeconds = 0.f;
}

FTimeHMS ACrowdFlowSimulationState::GetTimeInHMS()
{
    int32 TotalSeconds = FMath::FloorToInt(TimerInSeconds);
    int32 Hours = TotalSeconds / 3600;
    int32 Minutes = (TotalSeconds % 3600) / 60;
    int32 RemainingSeconds = (TotalSeconds % 3600) % 60;

    FTimeHMS TimeHMS;
    TimeHMS.Hours = Hours;
    TimeHMS.Minutes = Minutes;
    TimeHMS.Seconds = RemainingSeconds;

    return TimeHMS;
}

void ACrowdFlowSimulationState::SubmitAgentData(FAgentData AgentData)
{
    AgentDataArray.Add(AgentData);
}

