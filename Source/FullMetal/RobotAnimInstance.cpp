// Fill out your copyright notice in the Description page of Project Settings.

#include "RobotAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Vanguard.h"

URobotAnimInstance::URobotAnimInstance()
{
	// AwakeMontage�� �ε��� �����Ѵ�.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("AnimMontage'/Game/Animations/BattleRobot_Skeleton_Montage.BattleRobot_Skeleton_Montage'"));
	if (AM.Succeeded())
	{
		_AwakeMontage = AM.Object;
	}
}

void URobotAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// �ִϸ��̼� ����� �ʿ��� �����͸� �����Ѵ�.
	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn))
	{
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			_Velocity = Character->GetVelocity();

			_IsFalling = Character->GetMovementComponent()->IsFalling();

			FVector Accel = Character->GetCharacterMovement()->GetCurrentAcceleration();
			_ShouldMove = (_Velocity.Size2D() > 3.f) && (Accel.X != 0 || Accel.Y != 0);
		}
	}
}

void URobotAnimInstance::PlayAwakeMontage()
{
	if (!Montage_IsPlaying(_AwakeMontage))
	{
		Montage_Play(_AwakeMontage, 1.f);
	}
}
