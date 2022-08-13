// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RobotAIController.generated.h"

/**
 * 
 */
UCLASS()
class FULLMETAL_API ARobotAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARobotAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
private:
	UPROPERTY()
	class UBehaviorTree* _BehaviorTree;

	UPROPERTY()
	class UBlackboardData* _BlackboardData;
};
