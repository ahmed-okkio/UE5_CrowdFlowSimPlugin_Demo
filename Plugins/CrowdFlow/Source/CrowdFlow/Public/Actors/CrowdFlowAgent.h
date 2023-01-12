// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowdFlowAgent.generated.h"

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

	UPROPERTY(EditAnywhere)
	class UStaticMesh* StaticMesh;

	FTimerHandle TH_Movement;
	void UpdateMovement(FVector Direction, int32 Units);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SphereComponent;

	FVector ExitLocation = FVector(2084.618557,2006.420626,14.686587);
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	int32 GetDistanceToExit();
};
