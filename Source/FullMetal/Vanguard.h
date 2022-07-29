// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Vanguard.generated.h"

UCLASS()
class FULLMETAL_API AVanguard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVanguard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	void ForwardBackward(float Value);
	void RightLeft(float Value);
	void TurnRightLeft(float Value);

	UFUNCTION()
	void OnAwakeMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	class USpringArmComponent* _SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	class UCameraComponent* _Camera;

	UPROPERTY()
	class URobotAnimInstance* _AnimInstance;

	int32 _YawAmount = 0;
};
