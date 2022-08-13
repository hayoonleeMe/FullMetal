// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindCanFire.generated.h"

/**
 * 
 */
UCLASS()
class FULLMETAL_API UBTService_FindCanFire : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FindCanFire();
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
