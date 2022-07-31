// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RobotAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FULLMETAL_API URobotAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	URobotAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
public:
	void PlayAwakeMontage();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Robot, meta = (AllowPrivateAccess = true))
	FVector _Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Robot, meta = (AllowPrivateAccess = true))
	bool _IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Robot, meta = (AllowPrivateAccess = true))
	bool _ShouldMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Robot, meta = (AllowPrivateAccess = true))
	UAnimMontage* _AwakeMontage;
};
