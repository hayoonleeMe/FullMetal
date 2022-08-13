// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_WaitAndCheck.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_WaitAndCheck::UBTTask_WaitAndCheck()
{
	NodeName = TEXT("WaitAndCheck");
	bNotifyTick = true;
}

void UBTTask_WaitAndCheck::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// 도중에 공격할 대상을 찾으면 중지
	if (OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
