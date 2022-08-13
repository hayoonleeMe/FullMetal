// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"

ARobotAIController::ARobotAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("BehaviorTree'/Game/AI/BT_Vanguard.BT_Vanguard'"));
	if (BT.Succeeded())
	{
		_BehaviorTree = BT.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BD(TEXT("BlackboardData'/Game/AI/BB_Vanguard.BB_Vanguard'"));
	if (BD.Succeeded())
	{
		_BlackboardData = BD.Object;
	}
}

void ARobotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UBlackboardComponent* BlackboardTemp = Blackboard.Get();
	bool bUseBlackboard = UseBlackboard(_BlackboardData, BlackboardTemp);
	Blackboard = BlackboardTemp;
	if (bUseBlackboard)
	{
		if (RunBehaviorTree(_BehaviorTree))
		{
			// TODO
		}
	}
}

void ARobotAIController::OnUnPossess()
{
	Super::OnUnPossess();
}