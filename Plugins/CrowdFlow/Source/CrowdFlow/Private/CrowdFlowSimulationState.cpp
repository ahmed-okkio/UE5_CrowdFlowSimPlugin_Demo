// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdFlowSimulationState.h"
#include "Actors/CrowdFlowAgent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Kismet/GameplayStatics.h"

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

    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrowdFlowAgent::StaticClass(), AllActors);

    if (AllActors.Num() == 1)
    {
        WriteAgentDataToFile();
    }
}

void ACrowdFlowSimulationState::WriteAgentDataToFile()
{
    // Construct the output file path
    FString OutputFilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("ResultsData.txt"));

    FString OutputFileContents = TEXT("AgentName,StartTime,EndTime,Duration,UnitsTravelled,AverageUnitsPerSecond,StartingDistanceFromDest\n");

    // Open the output file for writing
    
    for (const FAgentData& AgentData : AgentDataArray)
    {
        // Format the agent data as a string and append it to the output file contents
        FString AgentDataString = FString::Printf(TEXT("%s,%02d:%02d:%02d,%02d:%02d:%02d,%02d:%02d:%02d,%d,%.2f,%.2f\n"),
            *AgentData.AgentName,
            AgentData.StartTime.Hours, AgentData.StartTime.Minutes, AgentData.StartTime.Seconds,
            AgentData.EndTime.Hours, AgentData.EndTime.Minutes, AgentData.EndTime.Seconds,
            AgentData.Duration.Hours, AgentData.Duration.Minutes, AgentData.Duration.Seconds,
            AgentData.UnitsTraveled,
            AgentData.AverageUnitsPerSecond,
            AgentData.StartingDistanceFromDest);
        OutputFileContents += AgentDataString;
    }

    FFileHelper::SaveStringToFile(OutputFileContents, *OutputFilePath);
}
