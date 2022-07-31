// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "Blueprint/UserWidget.h"

AMyGameMode::AMyGameMode()
{
	// 화면에 표시할 HUD 로드, 생성
	static ConstructorHelpers::FClassFinder<UUserWidget> HC(TEXT("WidgetBlueprint'/Game/UI/WBP_HUD.WBP_HUD_C'"));
	if (HC.Succeeded())
	{
		_HUDClass = HC.Class;
		_HUD = CreateWidget(GetWorld(), _HUDClass);
	}
}

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (_HUD != nullptr)
	{
		_HUD->AddToViewport();
	}
}