// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/CrowdFlowAgent.h"
#include "CrowdFlowAgentATC.generated.h"

/**
 * 
 */
UCLASS()
class CROWDFLOW_API ACrowdFlowAgentATC : public ACrowdFlowAgent
{
	GENERATED_BODY()

public:

	virtual void SeeExitSign(ACrowdFlowExitSign* ExitSign) override;
	
};
