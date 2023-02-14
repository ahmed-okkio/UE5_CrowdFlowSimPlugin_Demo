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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementBlocked);


UCLASS()
class ACrowdFlowAgent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrowdFlowAgent();

protected:
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

	UPROPERTY()	
	FMovementFinished MovementFinishedDelegate;
	UPROPERTY()
	FMovementBlocked MovementBlockedDelegate;

	EMovementModes MovementMode = EMovementModes::MM_Direct;
	
	FMove NextMove;
	
	TArray<FMove> PossibleMoves;
	
	ACrowdFlowExitSign* VisibleExitSign = nullptr;

	ACrowdFlowExitSign* LastVisibleExitSign = nullptr;

	ACrowdFlowExitStaircase* CurrentStaircase = nullptr;
	
	FTimerHandle TH_Movement;
	
	FTimerHandle TH_DirectMove;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveTowardsDirection(FVector Direction, int32 Units);

	void MoveToLocation(const FVector Destination);
	
	bool IsExitVisible() const;
	
	void AttemptDirectMoveToExit();

	void BeginLookingForDirectMoveToExit();
	
	void CalculateNextMove();
	
	bool IsBestMove(FMove NewMove) const;
	
	void ExecuteNextMove();

	UFUNCTION()
	void MoveTillUnitAmount(FVector Direction);

	UFUNCTION()
	void MoveTillBlocked(FVector Direction);

	UFUNCTION()
	void OnReachedExit();

	UFUNCTION()
	void OnFoundRightMostWall();

public:	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	int32 GetCurrentUnitsLeft();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SphereComponent;

	FVector ExitLocation;
	
	int32 CurrentUnitsLeft = 0;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	int32 GetDistanceToExit();


	void MoveToExit(ACrowdFlowExitSign* ExitSign);

	void MoveDownStair(ACrowdFlowExitStaircase* Staircase, bool RightStaircase);

	void MoveDownRightStair();
	
	void FindRightMostWall();

	void MoveDownLeftStair();

};
