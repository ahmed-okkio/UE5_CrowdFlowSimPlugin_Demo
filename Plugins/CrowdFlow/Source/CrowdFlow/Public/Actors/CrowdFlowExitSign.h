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
	UPROPERTY(EditAnywhere)
	class ATriggerVolume* TriggerVolume;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};