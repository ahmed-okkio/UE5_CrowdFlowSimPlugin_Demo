// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowdFlowAgent.generated.h"

class ACrowdFlowExitSign;
class ACrowdFlowExitStaircase;


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

UENUM()
enum class EMovementModes : uint8
{
	MM_Direct UMETA(DisplayName = "Direct"),
	MM_Exit UMETA(DisplayName = "Exit"),
};	

UENUM(BlueprintType)
enum class EAgentBehaviour : uint8
{
	FollowTheCrowd UMETA(DisplayName = "Follow The Crowd"),
	AvoidTheCrowd UMETA(DisplayName = "Avoid The Crowd"),
	AvoidUnknownExits UMETA(DisplayName = "Avoid Unkown Exits")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementBlocked);


UCLASS()
class CROWDFLOW_API ACrowdFlowAgent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrowdFlowAgent();

protected:

	UPROPERTY(EditDefaultsOnly)
	EAgentBehaviour AgentBehaviour = EAgentBehaviour::FollowTheCrowd;

	UPROPERTY(EditDefaultsOnly)
	float SameFloorHeightMargin = 50.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxStepHeight = 25.f;

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
	float UnitsToMovePastExit = 50.0f;

	float SphereRadius;

	float PersonalSpace = 50.0f;

	bool LookingForExit = false;

	bool FoundDirectMoveToExit = false;

	UPROPERTY()	
	FMovementFinished MovementFinishedDelegate;
	UPROPERTY()
	FMovementBlocked MovementBlockedDelegate;

	EMovementModes MovementMode = EMovementModes::MM_Direct;
	
	FMove NextMove;

	FMove* ActiveMove = nullptr;
	
	TQueue<FMove*> MoveQueue;
	
	ACrowdFlowExitSign* VisibleExitSign = nullptr;

	ACrowdFlowExitSign* LastVisibleExitSign = nullptr;

	ACrowdFlowExitStaircase* CurrentStaircase = nullptr;
	
	FTimerHandle TH_Movement;
	
	FTimerHandle TH_DirectMove;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsGrounded();

	void LookForPathAround();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveTowardsDirection(FVector Direction, int32 Units);

	void MoveToLocation(const FVector Destination);
	
	bool IsExitVisible() const;

	bool IsExitOnSameFloor() const;
	
	void AttemptDirectMoveToExit();

	void BeginLookingForDirectMoveToExit();
	
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
	void MoveTillUnitAmount(FVector Direction);

	UFUNCTION()
	void MoveTillBlocked(FVector Direction);

	UFUNCTION()
	void OnReachedExit();

	UFUNCTION()
	void OnFoundRightMostWall();


	UFUNCTION()
	void OnFoundLeftMostWall();

public:	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	int32 GetCurrentUnitsLeft();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SphereComponent;

	FVector ExitLocation;

	FVector ExitLocation1;

	
	int32 CurrentUnitsLeft = 0;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	int32 GetDistanceToExit();

	virtual void SeeExit(ACrowdFlowExitSign* ExitSign);

	void MoveToExit(ACrowdFlowExitSign* ExitSign);

	void MoveDownStair(ACrowdFlowExitStaircase* Staircase, bool RightStaircase);
};
