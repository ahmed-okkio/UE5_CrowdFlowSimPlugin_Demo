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

	class UArrowComponent* ForwardArrow;

	UPROPERTY(EditAnywhere)
	FVector BoundingBox;

	FTimerHandle TH_AgentTrace;

	UPROPERTY(EditDefaultsOnly)
	float TraceRate = 0.2f;

	void BeginTraceForAgents();

	void TraceForAgents();

	void StopTraceForAgents();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
