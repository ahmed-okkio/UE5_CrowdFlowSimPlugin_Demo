// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowdFlowAgent.generated.h"

class ACrowdFlowExitSign;

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

UCLASS()
class ACrowdFlowAgent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrowdFlowAgent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveTowardsDirection(FVector Direction, int32 Units);
	
	void MoveTowardsBestMove();
	
	bool IsBestMove(FMove NewMove) const;


	UPROPERTY(EditAnywhere)
	class UStaticMesh* StaticMesh;

	FTimerHandle TH_Movement;

	float TurnSmoothness = 8.0;

	float SphereRadius;
	
	UPROPERTY(EditInstanceOnly)
	float UnitsPerMove = 100.0;
	
	TArray<FMove> PossibleMoves;
	
	FMove BestMove;

	ACrowdFlowExitSign* VisibleExitSign = nullptr;

	void CalculatePossibleMoves();

	void SelectBestMove();

	UFUNCTION()
	void UpdateMovement(FVector Direction, int32 Units);

public:	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	int32 GetCurrentUnitsLeft();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SphereComponent;

	FVector ExitLocation = FVector(2084.618557, 3376.420626, 14.686587);
	
	int32 CurrentUnitsLeft = 0;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	int32 GetDistanceToExit();

	void MoveToLocation(const FVector Destination);

	void MoveToExitSign(ACrowdFlowExitSign* ExitSign);
};
