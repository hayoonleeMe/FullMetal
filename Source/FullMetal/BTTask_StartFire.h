// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StartFire.generated.h"

/**
 * 
 */
UCLASS()
class FULLMETAL_API UBTTask_StartFire : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_StartFire();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
