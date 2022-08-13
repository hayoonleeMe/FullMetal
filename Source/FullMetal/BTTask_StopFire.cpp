// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_StopFire.h"
#include "RobotAIController.h"
#include "Vanguard.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_StopFire::UBTTask_StopFire()
{
	NodeName = TEXT("StopFire");
}

EBTNodeResult::Type UBTTask_StopFire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AVanguard* Character = Cast<AVanguard>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Character)
		return EBTNodeResult::Failed;

	if (Character->IsShooting())
		Character->StopFire();

	return Result;
}