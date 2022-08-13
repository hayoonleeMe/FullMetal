// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SearchTarget.h"
#include "RobotAIController.h"
#include "Vanguard.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyStatComponent.h"

UBTService_SearchTarget::UBTService_SearchTarget()
{
	NodeName = TEXT("SearchTarget");
	// TODO : 총 발사 주기와 맞추도록 고려
	Interval = 0.15f;
	bNotifyBecomeRelevant = true;
}

void UBTService_SearchTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	// Tick 전에 _SearchRadius 초기화
	auto CurrentPawn = Cast<AVanguard>(OwnerComp.GetAIOwner()->GetPawn());
	if (!CurrentPawn)
		return;

	// 탐색범위를 무기 발사 최대 범위보다 약간 더 크게 설정
	_SearchRadius = CurrentPawn->GetStatComp()->GetMaxRange() * 1.5f;
}

void UBTService_SearchTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto CurrentPawn = Cast<AVanguard>(OwnerComp.GetAIOwner()->GetPawn());
	if (!CurrentPawn)
		return;

	UWorld* World = CurrentPawn->GetWorld();
	FVector Center = CurrentPawn->GetActorLocation();

	if (!World)
		return;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams(NAME_None, false, CurrentPawn);

	// ECC_GameTraceChannel5 : EnemyAttack
	// TODO : Target 찾는 범위 수정 필요 (전범위->시야기준)
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel5,
		FCollisionShape::MakeSphere(_SearchRadius),
		QueryParams);

	// Find Target
	if (bResult)
	{
		for (auto& OverlapResult : OverlapResults)
		{
			AVanguard* Character = Cast<AVanguard>(OverlapResult.GetActor());
			if (Character && Character->GetController()->IsPlayerController())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName(TEXT("Target")), Character);

				DrawDebugSphere(World, Center, _SearchRadius, 16, FColor::Green, false, 0.2f);

				UE_LOG(LogTemp, Warning, TEXT("Find Character"));

				return;
			}
		}
	}
	// No Target
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName(TEXT("Target")), nullptr);
		UE_LOG(LogTemp, Warning, TEXT("Target Off"));

		DrawDebugSphere(World, Center, _SearchRadius, 16, FColor::Red, false, 0.2f);
	}
}
