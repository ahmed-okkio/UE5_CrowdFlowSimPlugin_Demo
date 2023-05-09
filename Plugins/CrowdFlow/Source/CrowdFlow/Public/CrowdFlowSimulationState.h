// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CrowdFlowSimulationState.generated.h"

USTRUCT(BlueprintType)
struct FTimeHMS
{
	GENERATED_BODY()

	FTimeHMS()
	{
		Hours = 0;
		Minutes = 0;
		Seconds = 0;
	}
	FTimeHMS(int32 InHours, int32 InMinutes, int32 InSeconds)
	{
		Hours = InHours;
		Minutes = InMinutes;
		Seconds = InSeconds;
	}

	float GetTotalSeconds()
	{
		return Hours * 3600 + Minutes * 60 + Seconds;
	}

	UPROPERTY(BlueprintReadWrite, Category = "Time")
	int32 Hours;

	UPROPERTY(BlueprintReadWrite, Category = "Time")
	int32 Minutes;

	UPROPERTY(BlueprintReadWrite, Category = "Time")
	int32 Seconds;
};

USTRUCT(BlueprintType)
struct FAgentData
{
	GENERATED_BODY()

	FString AgentName;

	FTimeHMS StartTime;

	FTimeHMS EndTime;

	FTimeHMS Duration;

	float AverageSpeed;
	
	float WaitTime;

	float MaxSpeed;

	TMap<float, float> SpeedTimeData;

	float StartingDistanceFromDest;

	FTimeHMS GetEvacuationDuration()
	{
		int32 DurationInSeconds = ((EndTime.Hours - StartTime.Hours) * 3600) +
			((EndTime.Minutes - StartTime.Minutes) * 60) +
			(EndTime.Seconds - StartTime.Seconds);

		int32 DurationInHours = DurationInSeconds / 3600;
		int32 DurationInMinutes = (DurationInSeconds % 3600) / 60;
		int32 DurationInSecondsRemaining = (DurationInSeconds % 3600) % 60;

		return FTimeHMS(DurationInHours, DurationInMinutes, DurationInSecondsRemaining);
	}

	float GetDurationInSeconds()
	{
		int32 DurationInSeconds = Duration.Hours * 3600 + Duration.Minutes * 60 + Duration.Seconds;
		return static_cast<float>(DurationInSeconds);
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTImerTick,float,TimeInSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSubmittingResults);


/**
 * 
 */
UCLASS()
class CROWDFLOW_API ACrowdFlowSimulationState : public AGameStateBase
{
	GENERATED_BODY()


protected:
	float TimerInSeconds = 0.f;

	FTimerHandle TimerHandle;

	TArray<FAgentData> AgentDataArray;

	virtual void HandleBeginPlay() override;

public:
	UPROPERTY()
	FTImerTick TimerTickDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Simulation")
	FSubmittingResults OnSubmittingResults;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, float> LaneUsageData;


	UFUNCTION(BlueprintCallable, Category = "Write File")
	void WriteAgentDataToFile();

	void StartTimer();
	
	UFUNCTION()
	void TickTimer();

	void StopTimer();

	void ClearTimer();

	// Function to convert seconds to hours, minutes, and seconds
    UFUNCTION(BlueprintCallable, Category = "Time")
    FTimeHMS GetTimeInHMS();

	FTimeHMS GetTimeInHMS(float InSeconds);

	void SubmitAgentData(FAgentData AgentData);

	float GetTimeInSeconds() const;

	
};
