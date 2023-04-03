// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowAgentATC.h"
#include "Actors/CrowdFlowExitSign.h"


void ACrowdFlowAgentATC::SeeExit(ACrowdFlowExitSign* ExitSign)
{
	if (!ExitSign)
	{
		return;
	}

	if (!ExitSign->IsDefaultPath())
	{
		MoveToExit(ExitSign);
	}
}
