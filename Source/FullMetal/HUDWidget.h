// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class FULLMETAL_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindStat(class UMyStatComponent* StatComp);
	void UpdateHp();
	void UpdateRemainAmmo();
	void UpdateMaxAmmo();

	void Init();

private:
	TWeakObjectPtr<class UMyStatComponent> CurrentStatComp;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_HpBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TEXT_Hp;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TEXT_RemainAmmo;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TEXT_MaxAmmo;
};
