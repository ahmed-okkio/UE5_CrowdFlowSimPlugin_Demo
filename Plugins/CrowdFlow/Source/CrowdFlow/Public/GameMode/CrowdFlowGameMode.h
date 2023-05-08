// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CrowdFlowGameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimulationStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimulationEndDelegate);
class ACrowdFlowSimulationState;
/**
 * 
 */
UCLASS()
class CROWDFLOW_API ACrowdFlowGameMode : public AGameModeBase
{
	GENERATED_BODY()


private:
	bool bIsSimulationStarted = false;


protected:
	ACrowdFlowSimulationState* SimState = nullptr;

	virtual void InitGameState() override;
	virtual void StartPlay() override;
	virtual void BeginPlay() override;


public:
	UPROPERTY(EditDefaultsOnly, Category = "Simulation")
	bool StartSimulationOnBeginPlay = false;
	FSimulationStartDelegate OnSimulationStart;

	UPROPERTY(EditDefaultsOnly, Category = "Simulation")
	bool DebugModeAll = false;


	UFUNCTION(BlueprintCallable, Category = Simulation)
	void StartSimulation();

	UFUNCTION(BlueprintCallable, Category = Simulation)
	void StopSimulation();

	FSimulationStartDelegate GetSimulationStartDelegate();

	bool IsSimulationStarted() const;

	bool GetDebugMode() const;

	UFUNCTION(BlueprintCallable, Category = Simulation)
	void EnableDebugMode();




};
