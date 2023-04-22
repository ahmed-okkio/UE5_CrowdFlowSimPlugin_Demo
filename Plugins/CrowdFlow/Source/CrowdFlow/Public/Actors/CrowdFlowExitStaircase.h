// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowdFlowExitStaircase.generated.h"

UCLASS()
class CROWDFLOW_API ACrowdFlowExitStaircase : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ACrowdFlowExitStaircase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool ShouldTickIfViewportsOnly() const override;

#if WITH_EDITOR

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
#endif
	void BeginTraceForAgents();

	void TraceForAgents();

	FTimerHandle TH_AgentTrace;


	uint8 DepthPriority = 0;

	UPROPERTY(EditInstanceOnly, Category = Staircase)
	FVector BoundingBox = FVector(100,100,100);

	UPROPERTY(EditInstanceOnly, Category = Staircase)
	bool RightSideGoesDown = false;

	UPROPERTY(EditInstanceOnly, Category = Staircase)
	bool DrawDetectionDebug = true;
	
	UPROPERTY(EditInstanceOnly, Category = Staircase)
	bool SeeBoundingBoxThroughWalls = false;

	UPROPERTY(EditDefaultsOnly, Category = Staircase)
	float TraceRate = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = Staircase)
	class UBillboardComponent* SpriteComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsRightSideStaircase() const;

};
