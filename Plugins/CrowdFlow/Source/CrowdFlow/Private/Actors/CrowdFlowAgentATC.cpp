// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowAgentATC.h"
#include "Actors/CrowdFlowExitSign.h"


void ACrowdFlowAgentATC::SeeExitSign(ACrowdFlowExitSign* ExitSign)
{
	if (!ExitSign || FoundDirectMoveToExit)
	{
		return;
	}

	// If this is the first sign encountered
	if (!ExitSignBeingFollowed)
	{
		MoveToExit(ExitSign);
		return;
	}

	if (ExitSign->GetAgentCount(this) < ExitSignBeingFollowed->GetAgentCount(this))
	{
		MoveToExit(ExitSign);
	}

	/*else if (ExitSign->GetAgentCount(this) == ExitSignBeingFollowed->GetAgentCount(this))
	{
		if (ExitSignBeingFollowed->GetDistanceTo(this) < ExitSign->GetDistanceTo(this))
		{
			MoveToExit(ExitSignBeingFollowed);
		}
	}*/
}