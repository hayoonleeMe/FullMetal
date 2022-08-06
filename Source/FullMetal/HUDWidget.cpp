// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "MyStatComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

UHUDWidget::UHUDWidget()
{

}

void UHUDWidget::BindHp(UMyStatComponent* StatComp)
{
	CurrentStatComp = StatComp;
	StatComp->_OnHpChanged.AddUObject(this, &UHUDWidget::UpdateHp);
}

void UHUDWidget::UpdateHp()
{
	if (CurrentStatComp.IsValid())
	{
		PB_HpBar->SetPercent(CurrentStatComp->GetHpRatio());
		TEXT_Hp->SetText(FText::AsNumber(CurrentStatComp->GetHp()));
	}
}
