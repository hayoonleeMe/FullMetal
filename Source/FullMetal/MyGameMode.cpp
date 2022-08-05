#include "MyGameMode.h"
#include "Blueprint/UserWidget.h"

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

	// HUD 위젯을 화면에 표시한다.
	if (IsValid(_HUD))
	{
		_HUD->AddToViewport();
	}
}