// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CrowdFlowAgentATC.h"
#include "Actors/CrowdFlowExitSign.h"


void ACrowdFlowAgentATC::SeeExit(ACrowdFlowExitSign* ExitSign)
{
	if (!ExitSign)
	{
		return;
	}

	// If this is the first sign encountered
	if (VisibleExitSign == nullptr)
	{
		MoveToExit(ExitSign);
		FText();
		return;
	}

	if (VisibleExitSign != ExitSign)
	{
		if (VisibleExitSign->GetAgentCount() > ExitSign->GetAgentCount())
		{
			MoveToExit(ExitSign);
		}
		else if (VisibleExitSign->GetAgentCount() == ExitSign->GetAgentCount())
		{
			if (VisibleExitSign->IsKnownExit())
			{
				MoveToExit(VisibleExitSign);
			}
		}
	}

	

	
}