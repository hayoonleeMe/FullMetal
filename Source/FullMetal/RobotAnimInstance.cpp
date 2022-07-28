// Fill out your copyright notice in the Description page of Project Settings.

#include "RobotAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Vanguard.h"

void URobotAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn))
	{
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			_Velocity = Character->GetVelocity();

			_IsFalling = Character->GetMovementComponent()->IsFalling();

			FVector Accel = Character->GetCharacterMovement()->GetCurrentAcceleration();
			_ShouldMove = (_Velocity.Size2D() > 3.f) && (Accel.X != 0 && Accel.Y != 0);
		}
	}
}