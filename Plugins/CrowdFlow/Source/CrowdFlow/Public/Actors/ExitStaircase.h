// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExitStaircase.generated.h"

UCLASS()
class CROWDFLOW_API AExitStaircase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExitStaircase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool ShouldTickIfViewportsOnly() const override;

	void TraceForAgents();

	UPROPERTY(EditInstanceOnly)
	FVector BoundingBox;

	UPROPERTY(EditInstanceOnly)
	bool DrawDetectionDebug = false;

	UPROPERTY(EditDefaultsOnly)
	float TraceRate = 0.2f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
