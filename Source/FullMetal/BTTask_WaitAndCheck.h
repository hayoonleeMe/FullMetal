// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BTTask_WaitAndCheck.generated.h"

/**
 * 
 */
UCLASS()
class FULLMETAL_API UBTTask_WaitAndCheck : public UBTTask_Wait
{
	GENERATED_BODY()

public:
	UBTTask_WaitAndCheck();
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
