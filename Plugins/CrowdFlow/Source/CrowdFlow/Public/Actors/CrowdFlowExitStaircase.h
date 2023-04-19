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

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

	void BeginTraceForAgents();

	void TraceForAgents();

	FTimerHandle TH_AgentTrace;


	uint8 DepthPriority = 0;

	UPROPERTY(EditInstanceOnly)
	FVector BoundingBox = FVector(100,100,100);

	UPROPERTY(EditInstanceOnly)
	bool RightSideGoesDown = false;

	UPROPERTY(EditInstanceOnly)
	bool DrawDetectionDebug = true;
	
	UPROPERTY(EditInstanceOnly)
	bool SeeBoundingBoxThroughWalls = false;

	UPROPERTY(EditDefaultsOnly)
	float TraceRate = 0.2f;

	UPROPERTY(EditDefaultsOnly)
	class UBillboardComponent* SpriteComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsRightSideStaircase() const;

};
