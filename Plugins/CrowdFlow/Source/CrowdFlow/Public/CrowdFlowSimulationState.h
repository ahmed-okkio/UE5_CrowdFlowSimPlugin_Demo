// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CrowdFlowSimulationState.generated.h"



USTRUCT(BlueprintType)
struct FTimeHMS
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Time")
	int32 Hours;

	UPROPERTY(BlueprintReadWrite, Category = "Time")
	int32 Minutes;

	UPROPERTY(BlueprintReadWrite, Category = "Time")
	int32 Seconds;
};
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

	virtual void HandleBeginPlay() override;

public:

	void StartTimer();
	
	UFUNCTION()
	void TickTimer();

	void StopTimer();

	void ClearTimer();

	// Function to convert seconds to hours, minutes, and seconds
    UFUNCTION(BlueprintCallable, Category = "Time")
    FTimeHMS GetTimeInHMS();

	
};
