// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdFlowSimulationState.h"
#include "Actors/CFAgent.h"
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
    TimerTickDelegate.Broadcast(TimerInSeconds);
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
    return GetTimeInHMS(TimerInSeconds);
}

FTimeHMS ACrowdFlowSimulationState::GetTimeInHMS(float InSeconds)
{
    int32 TotalSeconds = FMath::FloorToInt(InSeconds);
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
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACFAgent::StaticClass(), AllActors);

    if (AllActors.Num() == 1)
    {
        OnSubmittingResults.Broadcast();
        WriteAgentDataToFile();
    }
}

float ACrowdFlowSimulationState::GetTimeInSeconds() const
{
    return TimerInSeconds;
}

void ACrowdFlowSimulationState::WriteAgentDataToFile()
{
    // Get the current date and time
    FDateTime Now = FDateTime::Now();

    // Construct the output file path with the current date and time as part of the file name
    FString OutputFilePath = FPaths::Combine(FPaths::ProjectDir(), FString::Printf(TEXT("Results/%04d%02d%02d_%02d%02d%02d/"),
        Now.GetYear(), Now.GetMonth(), Now.GetDay(),
        Now.GetHour(), Now.GetMinute(), Now.GetSecond()));


    FString OutputFileContents = TEXT("Red L,Red R,Yellow L,Yellow C,Yellow R,Blue L,Blue CL,Blue CR,Blue R\n");
    FString LaneUsageString = FString::Printf(TEXT("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n"),
        LaneUsageData["Red_L"], LaneUsageData["Red_R"], LaneUsageData["Yellow_L"], LaneUsageData["Yellow_C"], LaneUsageData["Yellow_R"], LaneUsageData["Blue_L"], LaneUsageData["Blue_CL"], LaneUsageData["Blue_CR"], LaneUsageData["Blue_R"]);
    FString LaneUsagePath = FPaths::Combine(OutputFilePath, FString::Printf(TEXT("LaneUsage.csv")));
    OutputFileContents += LaneUsageString;
    FFileHelper::SaveStringToFile(OutputFileContents, *LaneUsagePath);

    FString AgentDataCSV = TEXT("AgentName,StartTime,EndTime,Duration,AverageUnitsPerSecond,StartingDistanceFromDest,Wait Time,Max Speed\n");
    FString SpeedDataCSVHeader = TEXT("Time (s), Speed\n");

    for (const FAgentData& AgentData : AgentDataArray)
    {
        // Format the agent data as a string and append it to the output file contents
        FString AgentDataString = FString::Printf(TEXT("%s,%02d:%02d:%02d,%02d:%02d:%02d,%02d:%02d:%02d,%.2f,%.2f,%.2f,%.2f\n"),
            *AgentData.AgentName,
            AgentData.StartTime.Hours, AgentData.StartTime.Minutes, AgentData.StartTime.Seconds,
            AgentData.EndTime.Hours, AgentData.EndTime.Minutes, AgentData.EndTime.Seconds,
            AgentData.Duration.Hours, AgentData.Duration.Minutes, AgentData.Duration.Seconds,
            AgentData.AverageSpeed,
            AgentData.StartingDistanceFromDest,
            AgentData.WaitTime,AgentData.MaxSpeed);
        AgentDataCSV += AgentDataString;

        // Append speed data to a separate CSV file named after the agent
        FString SpeedDataCSV = SpeedDataCSVHeader;
        for (auto& SpeedTimePair : AgentData.SpeedTimeData)
        {
            FTimeHMS Time = GetTimeInHMS(SpeedTimePair.Key);
            float Speed = SpeedTimePair.Value;
            FString SpeedDataString = FString::Printf(TEXT("%02d:%02d:%02d,%.2f,\n"),
                Time.Hours, Time.Minutes, Time.Seconds, Speed);
            SpeedDataCSV += SpeedDataString;
        }
        FString SpeedDataFilePath = FPaths::Combine(OutputFilePath, FString::Printf(TEXT("%s_SpeedData.csv"), *AgentData.AgentName));
        FFileHelper::SaveStringToFile(SpeedDataCSV, *SpeedDataFilePath);
    }
    OutputFilePath = FPaths::Combine(OutputFilePath, FString::Printf(TEXT("ResultData.csv")));
    
    FFileHelper::SaveStringToFile(AgentDataCSV, *OutputFilePath);
}
