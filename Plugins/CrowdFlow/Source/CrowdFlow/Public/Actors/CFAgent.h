// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CFAgent.generated.h"

UCLASS()
class CROWDFLOW_API ACFAgent : public ACharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACFAgent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetFinalDestination() const;

protected:

	FVector FinalDestination;

};
