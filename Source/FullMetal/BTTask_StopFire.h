// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StopFire.generated.h"

/**
 * 
 */
UCLASS()
class FULLMETAL_API UBTTask_StopFire : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_StopFire();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
