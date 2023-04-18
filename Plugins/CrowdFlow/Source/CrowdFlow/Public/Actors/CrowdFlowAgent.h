// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowdFlowSimulationState.h"
#include "CrowdFlowAgent.generated.h"

class ACrowdFlowExitSign;
class ACrowdFlowExitStaircase;
class ACrowdFlowGameMode;

USTRUCT()
struct FMove
{
	GENERATED_BODY()

	FVector Direction;
	int32 Units;

	FMove()
	{
		Direction = FVector(0, 0, 0);
		Units = 0;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementBlocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTelemetryTimer);

UCLASS()
class CROWDFLOW_API ACrowdFlowAgent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrowdFlowAgent();

protected:
	static float SphereRadius;

	UPROPERTY(EditDefaultsOnly)
	float SameFloorHeightMargin = 200.f;

	UPROPERTY(EditInstanceOnly)
	float UnitsPerMove = 100.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float ExitReachedRange = 100.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float DirectMoveSearchRate=  0.5f;

	UPROPERTY(EditDefaultsOnly)
	float Speed = 0.005;

	UPROPERTY(EditDefaultsOnly)
	float TurnSmoothness = 8.0f;

	UPROPERTY(EditDefaultsOnly)
	float UnitsToMovePastExit = 500.0f;

	float PersonalSpace = 50.0f;

	bool FoundDirectMoveToExit = false;

	bool GoingAround = false;

	bool WaitingForStairCase = false;

	float TotalUnits = 0.0f;


	// Event delegates
	UPROPERTY()	
	FMovementFinished MovementFinishedDelegate;
	UPROPERTY()
	FMovementBlocked MovementBlockedDelegate;
	UPROPERTY()
	FTelemetryTimer OnTimerEnded;
	
	ACrowdFlowGameMode* GameMode = nullptr;

	ACrowdFlowExitSign* ExitSignBeingFollowed = nullptr;
	ACrowdFlowExitSign* LastFollowedExitSign = nullptr;
	ACrowdFlowExitStaircase* CurrentStaircase = nullptr;

	
	FTimerHandle TH_GoAround;
	FTimerHandle TH_Movement;
	FTimerHandle TH_DirectMove;

	FAgentData AgentData;

	FMove NextMove;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetNearestExitLocation();

	bool IsGrounded();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveTowardsDirection(FVector Direction, int32 Units);

	void MoveToLocation(const FVector Destination);
	
	bool IsFinalDestinationVisible() const;

	bool IsExitOnSameFloor(FVector ExitLocation) const;
	
	void AttemptDirectMoveToFinalDestination();

	void BeginLookingForDirectMoveToFinalDestination();
	
	void CalculateNextMove();
	
	bool IsBestMove(FMove NewMove) const;
	
	void ExecuteNextMove();

	void ClearMoveQueue();

	void MoveDownRightStair();

	void FindRightMostWall();

	void FollowRightMostWall();

	void MoveDownLeftStair();

	void FindLeftMostWall();

	void FollowLeftMostWall();

	UFUNCTION()
	void BeginGoAround(FVector Direction);

	UFUNCTION()
	void GoAround(FVector Direction);

	UFUNCTION()
	void StartSimulating();

	UFUNCTION()
	void MoveTillUnitAmount(FVector Direction);

	UFUNCTION()
	void MoveTillBlocked(FVector Direction);

	UFUNCTION()
	void OnReachedExit();

	UFUNCTION()
	void OnLeftExit();

	UFUNCTION()
	void OnFoundRightMostWall();

	UFUNCTION()
	void OnFoundLeftMostWall();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SphereComponent;

	FVector FinalDestination;

	FVector NearestExitLocation;

	int32 CurrentUnitsLeft = 0;

	int32 GoAroundUnits = 0;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	int32 GetDistanceToFinalDestination();

	virtual void SeeExitSign(ACrowdFlowExitSign* ExitSign);

	void MoveToExit(ACrowdFlowExitSign* ExitSign);

	void MoveDownStair(ACrowdFlowExitStaircase* Staircase, bool RightStaircase);

	static float GetSphereRadius();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	int32 GetCurrentUnitsLeft();

	bool IsWaitingForStairCase() const;

	bool IsOnStairCase() const;

};
