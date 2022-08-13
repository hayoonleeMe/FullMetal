#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Vanguard.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnFireEnded);

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

	UFUNCTION()
	void OnAwakeMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	class UMyStatComponent* GetStatComp() { return _Stat; }
	bool IsShooting() { return _bIsShooting; }

	void Fire();
	void StartFire();
	void StopFire();

	void Reload();

private:
	void ForwardBackward(float Value);
	void RightLeft(float Value);
	void TurnRightLeft(float Value);
	void LookUpDown(float Value);

	bool GunTrace(FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	class USpringArmComponent* _SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	class UCameraComponent* _Camera;

	UPROPERTY()
	class URobotAnimInstance* _AnimInstance;

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<AActor> _ProjectileClass;

	UPROPERTY(EditAnywhere, Category = Weapon)
	USoundBase* _FireSound;

	UPROPERTY(EditAnywhere, Category = Weapon)
	float _TimeBetweenShots;

	FTimerHandle _TimerHandle_HandleRefire;

	UPROPERTY(EditAnywhere, Category = Weapon)
	class UNiagaraSystem* _MuzzleEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	class UNiagaraComponent* _LeftMuzzleFlash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	class UNiagaraComponent* _RightMuzzleFlash;

	UPROPERTY(EditAnywhere, Category = Weapon)
	class UNiagaraSystem* _ImpactEffect;

	bool _bIsAwakeEnded = false;
	
	bool _bIsShooting = false;

	bool _bIsReloadEnded = true;

	FName _LeftMuzzleSocket;
	FName _RightMuzzleSocket;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UMyStatComponent* _Stat;

public: 
	float _ForwardBackwardValue = 0;

	float _RightLeftValue = 0;

	FRotator _UpDown;

	FOnFireEnded _OnFireEnded;
};
