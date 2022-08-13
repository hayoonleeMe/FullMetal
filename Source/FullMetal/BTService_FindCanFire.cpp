// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindCanFire.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Vanguard.h"
#include "RobotAIController.h"
#include "MyStatComponent.h"

UBTService_FindCanFire::UBTService_FindCanFire()
{
	NodeName = TEXT("FindCanFire");
	// TODO : 총 발사 주기와 맞추도록 고려
	Interval = 0.15f;
}

void UBTService_FindCanFire::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto CurrentPawn = Cast<AVanguard>(OwnerComp.GetAIOwner()->GetPawn());
	if (!CurrentPawn)
		return;

	auto Target = Cast<AVanguard>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (!Target)
		return;

	// Target이 무기 사거리에 들어와있는지 체크
	bool bCanFire = CurrentPawn->GetDistanceTo(Target) <= CurrentPawn->GetStatComp()->GetMaxRange();

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("CanFire"), bCanFire);
}
