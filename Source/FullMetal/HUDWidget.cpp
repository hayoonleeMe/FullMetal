// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "MyStatComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UHUDWidget::Init()
{
	// 위젯 초기화
	UpdateHp();
	UpdateRemainAmmo();
	UpdateMaxAmmo();
}

void UHUDWidget::BindStat(UMyStatComponent* StatComp)
{
	CurrentStatComp = StatComp;
	StatComp->_OnHpChanged.AddUObject(this, &UHUDWidget::UpdateHp);
	StatComp->_OnRemainAmmoChanged.AddUObject(this, &UHUDWidget::UpdateRemainAmmo);
	StatComp->_OnMaxAmmoChanged.AddUObject(this, &UHUDWidget::UpdateMaxAmmo);
}

void UHUDWidget::UpdateHp()
{
	if (CurrentStatComp.IsValid())
	{
		PB_HpBar->SetPercent(CurrentStatComp->GetHpRatio());
		TEXT_Hp->SetText(FText::AsNumber(CurrentStatComp->GetHp()));
	}
}

// TODO : 텍스트 자리수 맞추기
void UHUDWidget::UpdateRemainAmmo()
{
	if (CurrentStatComp.IsValid())
	{
		TEXT_RemainAmmo->SetText(FText::FromString(FString::Printf(TEXT("%d"), CurrentStatComp->GetRemainAmmo())));
	}
}

// TODO : 텍스트 자리수 맞추기
void UHUDWidget::UpdateMaxAmmo()
{
	if (CurrentStatComp.IsValid())
	{
		TEXT_MaxAmmo->SetText(FText::FromString(FString::Printf(TEXT(" / %d"), CurrentStatComp->GetMaxAmmo())));
	}
}