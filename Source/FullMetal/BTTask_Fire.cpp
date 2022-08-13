// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Fire.h"
#include "Vanguard.h"
#include "RobotAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Fire::UBTTask_Fire()
{
	NodeName = TEXT("StartFire");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Fire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Character = Cast<AVanguard>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Character)
		return EBTNodeResult::Failed;

	if (!Character->IsShooting())
		Character->StartFire();

	return EBTNodeResult::InProgress;
}

void UBTTask_Fire::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto CurrentPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!CurrentPawn)
		return;

	auto Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (!Target)
		return;

	// 총을 발사할 때 Target을 바라보도록 매 틱마다 설정
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(CurrentPawn->GetActorLocation(), Target->GetActorLocation());

	CurrentPawn->SetActorRotation(LookAtRot);


	if (!OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("CanFire")))
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
