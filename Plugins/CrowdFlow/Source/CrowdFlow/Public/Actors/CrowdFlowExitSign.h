// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowdFlowExitSign.generated.h"


UCLASS()
class CROWDFLOW_API ACrowdFlowExitSign : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrowdFlowExitSign();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostLoad() override;
	virtual void PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph) override;
	virtual void PostInitProperties() override;
	
	virtual void PostEditMove(bool bFinished);

	virtual void PostRegisterAllComponents() override;

	virtual bool ShouldTickIfViewportsOnly() const override;

	UPROPERTY(EditInstanceOnly)
	class UBillboardComponent* SpriteComponent;

	bool DetectionActive = false;

	UPROPERTY(EditInstanceOnly)
	bool DrawDetectionDebug = false;

	UPROPERTY(EditInstanceOnly)
	FVector BoundingBox;

	UPROPERTY(EditInstanceOnly)
	FVector Offset;

	FVector ExitSignAgentDestination;

	FTimerHandle TH_AgentTrace;

	UPROPERTY(EditInstanceOnly)
	float AgentDestinationDistanceFromSign = 150.0f;

	UPROPERTY(EditDefaultsOnly)
	float TraceRate = 0.2f;

	void BeginTraceForAgents();

	void TraceForAgents();

	void StopTraceForAgents();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetExitSignDestination() const;

};
