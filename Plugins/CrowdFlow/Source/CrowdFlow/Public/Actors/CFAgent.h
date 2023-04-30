// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CrowdFlowSimulationState.h"
#include "CFAgent.generated.h"

class AAIController;
UCLASS()
class CROWDFLOW_API ACFAgent : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = Agent)
	float SameFloorHeightMargin = 200.f;

	FVector NearestExitLocation;

	FAgentData AgentData;


public:	
	// Sets default values for this actor's properties
	ACFAgent();

	virtual void BeginPlay() override;

	void StartSimulating();
	void MoveToLocation(FVector& Loc);
	FVector GetNearestExitLocation();
	bool IsExitOnSameFloor(FVector ExitLocation) const;
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetFinalDestination() const;

protected:

	FVector FinalDestination;

};
