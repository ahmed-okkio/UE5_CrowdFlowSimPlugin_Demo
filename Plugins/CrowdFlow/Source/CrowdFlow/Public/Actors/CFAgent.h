// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CrowdFlowSimulationState.h"
#include "CFAgent.generated.h"

class AAIController;
class ACrowdFlowExitSign;
class ACrowdFlowExitStaircase;
UCLASS()
class CROWDFLOW_API ACFAgent : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = Agent)
	float SameFloorHeightMargin = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = Agent)
	float DirectMoveSearchRate = 0.5f;

	float WaitTime = 0.f;
	bool FoundDirectMoveToExit = false;

	ACrowdFlowSimulationState* SimState = nullptr;

	FVector NearestExitLocation;

	ACrowdFlowExitSign* ExitSignBeingFollowed = nullptr;
	ACrowdFlowExitSign* LastFollowedExitSign = nullptr;

	ACrowdFlowExitStaircase* CurrentStaircase = nullptr;


	FAgentData AgentData;

	FTimerHandle TH_GoAround;
	FTimerHandle TH_DirectMove;
	FTimerHandle TH_TrackTime;


public:	
	// Sets default values for this actor's properties
	ACFAgent();

	virtual void BeginPlay() override;

	void StartSimulating();
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	void EndSimulation();
	void MoveToLocation(FVector Loc);
	void StopMovement();
	UFUNCTION()
	void RegisterSpeedAtTime(float TimeInSeconds);
	void BeginLookingForDirectMoveToFinalDestination();
	void AttemptDirectMoveToFinalDestination();
	bool IsFinalDestinationVisible();
	FVector GetNearestExitLocation();
	bool IsExitOnSameFloor(FVector ExitLocation) const;
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetFinalDestination() const;
	virtual void SeeExitSign(ACrowdFlowExitSign* ExitSign);
	bool IsExitSignBehind(ACrowdFlowExitSign* ExitSign) const;
	void MoveDownStair(ACrowdFlowExitStaircase* Staircase);



protected:

	FVector FinalDestination;

};
