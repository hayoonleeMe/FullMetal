#include "MyStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"
#include "Vanguard.h"

// Sets default values for this component's properties
UMyStatComponent::UMyStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bWantsInitializeComponent = true;

	_Class = 1;	// Vanguard
}

// Called when the game starts
void UMyStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMyStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Vanguard의 스탯으로 초기화
	SetClass(_Class);
}

void UMyStatComponent::SetClass(int32 NewClass)
{
	auto MyGameInstance = Cast<UMyGameInstance>((UGameplayStatics::GetGameInstance(GetWorld())));
	if (IsValid(MyGameInstance))
	{
		auto StatData = MyGameInstance->GetStatData(NewClass);
		if (StatData)
		{
			_Class = StatData->_Class;
			_MaxHp = StatData->_MaxHp;
			SetHp(StatData->_MaxHp);
			_Speed = StatData->_Speed;
			_PrimaryDamage = StatData->_PrimaryDamage;
			_RpmPercent = StatData->_RpmPercent;
			SetMaxAmmo(StatData->_MaxAmmo);
			_AmmoCapacity = StatData->_AmmoCapacity;
			SetRemainAmmo(StatData->_AmmoCapacity);
		}
	}
}

void UMyStatComponent::OnAttacked(float DamageAmount)
{
	int32 NewHp = _Hp - DamageAmount;
	SetHp(NewHp);
}

void UMyStatComponent::SetHp(int32 NewHp)
{
	_Hp = NewHp;
	if (_Hp < 0)
		_Hp = 0;

	_OnHpChanged.Broadcast();
}

void UMyStatComponent::OnFired()
{
	int32 NewAmmo = _RemainAmmo - 1;
	SetRemainAmmo(NewAmmo);
}

void UMyStatComponent::SetMaxAmmo(int32 NewAmmo)
{
	_MaxAmmo = NewAmmo;

	_OnMaxAmmoChanged.Broadcast();
}

void UMyStatComponent::SetRemainAmmo(int32 NewAmmo)
{
	_RemainAmmo = NewAmmo;

	if (_RemainAmmo <= 0)
	{
		// 재장전한다.
		auto Character = Cast<AVanguard>(GetOwner());
		if (Character)
		{
			Character->Reload();
		}
	}

	_OnRemainAmmoChanged.Broadcast();
}

void UMyStatComponent::OnReloaded()
{
	int32 NewAmmo = FMath::Min(_MaxAmmo, _AmmoCapacity - _RemainAmmo);

	SetRemainAmmo(_RemainAmmo + NewAmmo);
	SetMaxAmmo(_MaxAmmo - NewAmmo);
}