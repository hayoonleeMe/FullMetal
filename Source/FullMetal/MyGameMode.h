#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FULLMETAL_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMyGameMode();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> _HUDClass;

	UPROPERTY()
	UUserWidget* _HUD;
};
