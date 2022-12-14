#include "MyGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Vanguard.h"
#include "HUDWidget.h"
#include "Kismet/GameplayStatics.h"

AMyGameMode::AMyGameMode()
{
	// HUD 위젯 로드
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

	// HUD 위젯을 캐릭터의 스탯과 연동한다.
	// 게임이 시작되어야 PlayerPawn이 Spawn되므로 UGameplayStatics::GetPlayerPawn은 최소 BeginPlay에서 호출해야함
	auto Character = Cast<AVanguard>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Character)
	{
		auto StatComp = Character->GetStatComp();
		if (StatComp)
		{
			auto HUDWidget = Cast<UHUDWidget>(_HUD);
			if (HUDWidget)
			{
				HUDWidget->BindStat(StatComp);
				HUDWidget->Init();
			}
		}
	}

	// HUD 위젯을 화면에 표시한다.
	if (IsValid(_HUD))
	{
		_HUD->AddToViewport();
	}
}