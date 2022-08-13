#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpChanged);
DECLARE_MULTICAST_DELEGATE(FOnRemainAmmoChanged);
DECLARE_MULTICAST_DELEGATE(FOnMaxAmmoChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FULLMETAL_API UMyStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:	
	void SetClass(int32 NewClass);
	void OnAttacked(float DamageAmount);
	void SetHp(int32 NewHp);
	void OnFired();
	void SetRemainAmmo(int32 NewAmmo);
	void SetMaxAmmo(int32 NewAmmo);
	void OnReloaded();

	int32 GetClass() { return _Class; }
	int32 GetHp() { return _Hp; }
	int32 GetMaxHp() { return _MaxHp; }
	float GetHpRatio() { return _Hp / (float)_MaxHp; }
	float GetSpeed() { return _Speed; }
	int32 GetPrimaryDamage() { return _PrimaryDamage; }
	float GetRpmPercent() { return _RpmPercent; }
	int32 GetMaxAmmo() { return _MaxAmmo; }
	int32 GetAmmoCapacity() { return _AmmoCapacity; }
	int32 GetRemainAmmo() { return _RemainAmmo; }
	float GetMaxRange() { return _MaxRange;	}

private:
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 _Class;

	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 _MaxHp;

	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 _Hp;

	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	float _Speed;

	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 _PrimaryDamage;

	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	float _RpmPercent;

	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 _MaxAmmo;

	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 _AmmoCapacity;

	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 _RemainAmmo;

	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	float _MaxRange;

public:
	FOnHpChanged _OnHpChanged;
	FOnRemainAmmoChanged _OnRemainAmmoChanged;
	FOnMaxAmmoChanged _OnMaxAmmoChanged;
};
