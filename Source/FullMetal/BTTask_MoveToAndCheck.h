// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToAndCheck.generated.h"

/**
 * 
 */
UCLASS()
class FULLMETAL_API UBTTask_MoveToAndCheck : public UBTTask_MoveTo
{
	GENERATED_BODY()
	
public:
	UBTTask_MoveToAndCheck();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
