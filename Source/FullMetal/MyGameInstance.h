// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "MyGameInstance.generated.h"

// TODO : 추후 무기 시스템이 분리되면 거기에 맞게 무기 데이터도 따로 구성해야한다.
USTRUCT()
struct FClassData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 _Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 _MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 _PrimaryDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _RpmPercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 _MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 _AmmoCapacity;
};

UCLASS()
class FULLMETAL_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMyGameInstance();

	FClassData* GetStatData(int32 Class);

	float GetMaxTimeBetweenFire() { return _MaxTimeBetweenFire; }

private:
	UPROPERTY()
	class UDataTable* MyStats;

	float _MaxTimeBetweenFire = 0.09f;
};
